/* bitskipc.c
 
   Bitskipc - an implementation of Bitskip in C.
   Bitskip is a simple ASIC miner
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <ftdi.h>

#include "serial_monitor.h"

int main(int argc, char **argv) {
    struct ftdi_context *ftdi;

    ftdi = open_serial();

    if (ftdi == NULL) {
        fprintf(stderr, "Failed to open serial port\\n");
        return -1;
    }

    pthread_t thread;

    // Start the serial thread
    pthread_create(&thread, NULL, serial_thread, ftdi);

    // Wait for the thread to finish
    pthread_join(thread, NULL);

    return 0;

}