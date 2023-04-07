#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <ftdi.h>

#define FTDI_VID 0x0403
#define FTDI_PID 0x6015
#define FTDI_BAUDRATE 115200

static int exitRequested = 0;

/*
 * SIGINT handler, so we can gracefully exit when the user hits ctrl-C.
 */
static void sigintHandler(int signum) {
    exitRequested = 1;
}

struct ftdi_context * open_serial(void) {
    int ret;

    // Initialize the FTDI context
    struct ftdi_context *ftdi = ftdi_new();
    if (ftdi == NULL) {
        fprintf(stderr, "ftdi_new failed\\n");
        return NULL;
    }

    // Open the USB device
    ret = ftdi_usb_open(ftdi, FTDI_VID, FTDI_PID);
    if (ret < 0) {
        fprintf(stderr, "unable to open device: %d (%s)\\n", ret, ftdi_get_error_string(ftdi));
        goto error;
    }

    // Set the baud rate
    ret = ftdi_set_baudrate(ftdi, FTDI_BAUDRATE);
    if (ret < 0) {
        fprintf(stderr, "unable to set baud rate: %d (%s)\\n", ret, ftdi_get_error_string(ftdi));
        goto error;
    }

    // Set the data bits, stop bits, and parity
    ret = ftdi_set_line_property(ftdi, BITS_8, STOP_BIT_1, NONE);
    if (ret < 0) {
        fprintf(stderr, "unable to set line property: %d (%s)\\n", ret, ftdi_get_error_string(ftdi));
        goto error;
    }

    return ftdi;

error:
    // Clean up the FTDI context
    ftdi_usb_close(ftdi);
    ftdi_free(ftdi);
    return NULL;
}

void *serial_thread(void *arg) {
    struct ftdi_context *ftdi;
    int ret, i;
    unsigned char buf[256];
    int retval = EXIT_FAILURE;

    ftdi = (struct ftdi_context*)arg;

    signal(SIGINT, sigintHandler);

    // Start reading data from the device
    while (!exitRequested) {
        ret = ftdi_read_data(ftdi, buf, sizeof(buf));
        if (ret < 0) {
            fprintf(stderr, "unable to read data: %d (%s)\\n", ret, ftdi_get_error_string(ftdi));
            goto error;
        }

        // Print the data to stdout
        fwrite(buf, ret, 1, stdout);
        fflush(stdout);

        // Wait for a bit before reading more data
        usleep(1000);
    }

    signal(SIGINT, SIG_DFL);
    retval =  EXIT_SUCCESS;

error:
    // Clean up the FTDI context
    ftdi_usb_close(ftdi);
    ftdi_free(ftdi);
    return NULL;
}


int set_rts(struct ftdi_context *ftdi, int state) {
    int ret;

    ret = ftdi_setrts(ftdi, state);
    if (ret < 0) {
        fprintf(stderr, "unable to set rts: %d (%s)\\n", ret, ftdi_get_error_string(ftdi));
        goto error;
    }

    return 0;  
error:
    // Clean up the FTDI context
    ftdi_usb_close(ftdi);
    ftdi_free(ftdi);
    return -1;
}