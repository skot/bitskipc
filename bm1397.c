#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "serial_monitor.h"

void send_chippy(struct ftdi_context *ftdi) {

    unsigned char chippy[7] = {0x55, 0xAA, 0x52, 0x05, 0x00, 0x00, 0x0A};
    //send serial data
    write_data(ftdi, chippy, sizeof(chippy));
}