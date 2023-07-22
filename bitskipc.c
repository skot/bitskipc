/* bitskipc.c
 
   Bitskipc - an implementation of Bitskip in C.
   Bitskip is a simple ASIC miner using FTDI usbserial
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <ftdi.h>
#include <string.h>

#include "serial_monitor.h"
#include "pretty.h"
#include "bm1366.h"

int main(int argc, char **argv) {
    struct ftdi_context *ftdi;

    //open serial port
    ftdi = open_serial();
    if (ftdi == NULL) {
        fprintf(stderr, "Failed to open serial port\n");
        return -1;
    }

    //toggle reset
    reset_BM1366(ftdi);

    send_first_thing(ftdi);
    send_first_thing(ftdi);
    send_first_thing(ftdi);

    send_read_address(ftdi);

    //read back data
    debug_serial_rx(ftdi);

    send_init(ftdi);

    send_work_manual(ftdi);

    while (1) {
        debug_serial_rx(ftdi);
    }

    ftdi_setrts(ftdi, 1);

    return 0;

}