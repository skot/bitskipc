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

#include "serial_monitor.h"
#include "pretty.h"
#include "bm1397.h"

int main(int argc, char **argv) {
    struct ftdi_context *ftdi;
    uint8_t buf[1024];
    uint16_t len;

    //open serial port
    ftdi = open_serial();
    if (ftdi == NULL) {
        fprintf(stderr, "Failed to open serial port\\n");
        return -1;
    }

    while(1) {
        send_chippy(ftdi);

        //read back data
        len = serial_thread(ftdi, buf);

        if (len < 0) {
            fprintf(stderr, "Failed to read serial port\n");
            return -1;
        } else {
            prettyHex(buf, len);
            printf("(%d)\n", len);
        }
    }

    return 0;

}