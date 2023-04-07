/* bitskipc.c
 
   Bitskipc - an implementation of Bitskip in C.
   Bitskip is a simple ASIC miner
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <ftdi.h>

#define FTDI_VID 0x0403
#define FTDI_PID 0x6015

static int exitRequested = 0;

/*
 * SIGINT handler, so we can gracefully exit when the user hits ctrl-C.
 */
static void sigintHandler(int signum) {
    exitRequested = 1;
}

int main(int argc, char **argv) {
    struct ftdi_context *ftdi;
    unsigned char buf[4] = {0x00, 0xFF, 0x00, 0xFF};
    int f = 0;
    int baudrate = 115200;
    int interface = INTERFACE_ANY;
    int retval = EXIT_FAILURE;


    // Init
    if ((ftdi = ftdi_new()) == 0) {
        fprintf(stderr, "ftdi_new failed\n");
        return EXIT_FAILURE;
    }

    // Select interface
    ftdi_set_interface(ftdi, interface);
    
    // Open device
    f = ftdi_usb_open(ftdi, FTDI_VID, FTDI_PID);
    
    if (f < 0) {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", f, ftdi_get_error_string(ftdi));
        exit(-1);
    }

    // Set baudrate
    f = ftdi_set_baudrate(ftdi, baudrate);
    if (f < 0) {
        fprintf(stderr, "unable to set baudrate: %d (%s)\n", f, ftdi_get_error_string(ftdi));
        exit(-1);
    }
    
    // Set line parameters
    f = ftdi_set_line_property(ftdi, 8, STOP_BIT_1, NONE);
    if (f < 0) {
        fprintf(stderr, "unable to set line parameters: %d (%s)\n", f, ftdi_get_error_string(ftdi));
        exit(-1);
    }

    signal(SIGINT, sigintHandler);

    while (!exitRequested) {
            f = ftdi_write_data(ftdi, buf, sizeof(buf));
            f = ftdi_read_data(ftdi, buf, sizeof(buf));
        if (f<0) {
            usleep(1 * 1000000);
        } else if (f> 0) {
            //fprintf(stderr, "read %d bytes\n", f);
            fwrite(buf, f, 1, stdout);
            fflush(stderr);
            fflush(stdout);
        }
    }

    signal(SIGINT, SIG_DFL);
    retval =  EXIT_SUCCESS;
            
    ftdi_usb_close(ftdi);
    do_deinit:
    ftdi_free(ftdi);

    return retval;
}