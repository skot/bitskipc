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
#include "bm1397.h"

int main(int argc, char **argv) {
    struct ftdi_context *ftdi;
    uint8_t buf[CHUNK_SIZE];
    uint16_t len;

    //open serial port
    ftdi = open_serial();
    if (ftdi == NULL) {
        fprintf(stderr, "Failed to open serial port\\n");
        return -1;
    }

    ftdi_setrts(ftdi, 0);

    send_chippy(ftdi);

    //read back data
    len = serial_rx(ftdi, buf);

    if (len < 0) {
        fprintf(stderr, "Failed to read serial port\n");
        return -1;
    } else if (len > 0) {
        prettyHex(buf, len);
        printf("(%d)\n", len);
    }

    send_init(ftdi);

    memset(buf, 0, 1024);

    send_work(ftdi);

    while (1) {
        len = serial_rx(ftdi, buf);

        if (len < 0) {
            fprintf(stderr, "Failed to read serial port\n");
            return -1;
        } else if (len > 0) {
            prettyHex(buf, len);
            printf("(%d)\n", len);
        }
    }

    ftdi_setrts(ftdi, 1);

    return 0;

}