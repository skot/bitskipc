#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <ftdi.h>

#include "pretty.h"

#define FTDI_VID 0x0403
#define FTDI_PID 0x6015
#define FTDI_BAUDRATE 115200

#define SERIAL_RX_TIME_MS 1000

static int exitRequested = 0;

/*
 * SIGINT handler, so we can gracefully exit when the user hits ctrl-C.
 */
static void sigintHandler(int signum) {
    exitRequested = 1;
}

/// @brief opens a ftdi serial port
/// @return a pointer to the ftdi context, or NULL on error
struct ftdi_context * open_serial(void) {
    int ret;

    // Initialize the FTDI context
    struct ftdi_context *ftdi = ftdi_new();
    if (ftdi == NULL) {
        fprintf(stderr, "ftdi_new failed\n");
        return NULL;
    }

    // Open the USB device
    ret = ftdi_usb_open(ftdi, FTDI_VID, FTDI_PID);
    if (ret < 0) {
        fprintf(stderr, "unable to open device: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
        goto error;
    }

    // Set the baud rate
    ret = ftdi_set_baudrate(ftdi, FTDI_BAUDRATE);
    if (ret < 0) {
        fprintf(stderr, "unable to set baud rate: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
        goto error;
    }

    // Set the data bits, stop bits, and parity
    ret = ftdi_set_line_property(ftdi, BITS_8, STOP_BIT_1, NONE);
    if (ret < 0) {
        fprintf(stderr, "unable to set line property: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
        goto error;
    }

    return ftdi;

error:
    // Clean up the FTDI context
    ftdi_usb_close(ftdi);
    ftdi_free(ftdi);
    return NULL;
}

/// @brief waits for a serial response from the device
/// @param ftdi open ftdi context  
/// @param buf bufffer to read data into
/// @return number of bytes read, or -1 on error
int16_t serial_thread(struct ftdi_context *ftdi, uint8_t * buf) {
    int ret;
    uint16_t timeout = 0;

    // Start reading data from the device
    while (timeout < SERIAL_RX_TIME_MS) {
        ret = ftdi_read_data(ftdi, buf, sizeof(buf));
        if (ret < 0) {
            fprintf(stderr, "unable to read data: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
            return -1;
        }

        if (ret > 0) {
            return ret;
        }

        // Print the data to stdout
        // fwrite(buf, ret, 1, stdout);
        // fflush(stdout);

        // Wait for a bit before reading more data
        usleep(1000);
        timeout++;
    }
    return 0;
}


/// @brief sets the rts line
/// @param ftdi open ftdi context
/// @param state state to set rts to
/// @return 0 on success, -1 on error
int set_rts(struct ftdi_context *ftdi, int state) {
    int ret;

    ret = ftdi_setrts(ftdi, state);
    if (ret < 0) {
        fprintf(stderr, "unable to set rts: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
        goto error;
    }

    return 0;  
error:
    // Clean up the FTDI context
    ftdi_usb_close(ftdi);
    ftdi_free(ftdi);
    return -1;
}


/// @brief writes data to the serial port
/// @param ftdi open ftdi context
/// @param buf buffer to write
/// @param len number of bytes to write
/// @return number of bytes written or -1 on error
int write_data(struct ftdi_context *ftdi, const unsigned char *buf, int len) {
    int ret, i;
    uint16_t status;
    uint16_t bytes_written;

    printf("->");
    prettyHex((unsigned char *)buf, len);
    printf("\n");

    // Write the data to the device
    ret = ftdi_write_data(ftdi, buf, len);
    if (ret < 0) {
        fprintf(stderr, "unable to write data: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
        return -1;
    }

    bytes_written = ret;

    // Wait for the data to be sent
    for (i = 0; i < 10; i++) {
        ret = ftdi_poll_modem_status(ftdi, &status);
        if (ret < 0) {
            fprintf(stderr, "unable to poll modem status: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
            return -1;
        }
        if (ret == 0) {
            // All data has been sent
            return bytes_written;
        }
        usleep(1000);
    }

    fprintf(stderr, "timed out waiting for data to be sent\n");
    return -1;
}

