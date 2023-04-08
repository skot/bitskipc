#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "serial_monitor.h"
#include "pretty.h"


int send_serial(struct ftdi_context *ftdi, unsigned char *buf, int len) {
    int ret;

    unsigned char *buf2 = malloc(len+3);

    //add the preamble and checksum
    buf2[0] = 0x55;
    buf2[1] = 0xAA;
    memcpy(buf2+2, buf, len);

    len += 2;

    printf("->");
    prettyHex(buf2, len);
    printf("\n");
    
    //write the data to the serial port
    ret = ftdi_write_data(ftdi, buf2, len);
    free(buf2);

    if (ret < 0) {
        fprintf(stderr, "ftdi_write_data failed, error %d (%s)", ret, ftdi_get_error_string(ftdi));
    }
    return ret;
}

void send_chippy(struct ftdi_context *ftdi) {

    unsigned char chippy[5] = {0x52, 0x05, 0x00, 0x00, 0x0A};
    //send serial data
    send_serial(ftdi, chippy, 5);
}

void send_init(struct ftdi_context *ftdi) {

    unsigned char chain_inactive[5] = {0x53, 0x05, 0x00, 0x00, 0x03};
    //send serial data
    send_serial(ftdi, chain_inactive, 5);
    send_serial(ftdi, chain_inactive, 5);
    send_serial(ftdi, chain_inactive, 5);

    unsigned char chippy2[5] = {0x40, 0x05, 0x00, 0x00, 0x1C};
    send_serial(ftdi, chippy2, 5);
    unsigned char init[9] = {0x51, 0x09, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1C}; //init1 - clock_order_control0
    send_serial(ftdi, init, 9);
    unsigned char init2[9] = {0x51, 0x09, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00, 0x11}; //init2 - clock_order_control1
    send_serial(ftdi, init2, 9);
    unsigned char init3[9] = {0x51, 0x09, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x02}; //init3 - ordered_clock_enable
    send_serial(ftdi, init3, 9);
    unsigned char init4[9] = {0x51, 0x09, 0x00, 0x3C, 0x80, 0x00, 0x80, 0x74, 0x10}; //init4 - init_4_?
    send_serial(ftdi, init4, 9);
    unsigned char set_ticket[9] = {0x51, 0x09, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x1C}; //set_ticket - ticket_mask
    send_serial(ftdi, set_ticket, 9);
    unsigned char init5[9] = {0x51, 0x09, 0x00, 0x68, 0xC0, 0x70, 0x01, 0x11, 0x00}; //init5 - pll3_parameter
    send_serial(ftdi, init5, 9);
    unsigned char init5_2[9] = {0x51, 0x09, 0x00, 0x68, 0xC0, 0x70, 0x01, 0x11, 0x00}; //init5 - pll3_parameter
    send_serial(ftdi, init5_2, 9);
    unsigned char init6[9] = {0x51, 0x09, 0x00, 0x28, 0x06, 0x00, 0x00, 0x0F, 0x18}; //init6 - fast_uart_configuration
    send_serial(ftdi, init6, 9);
    unsigned char baudrate[9] = {0x51, 0x09, 0x00, 0x18, 0x00, 0x00, 0x7A, 0x31, 0x15}; //baudrate - misc_control
    send_serial(ftdi, baudrate, 9);
    unsigned char prefreq1[9] = {0x51, 0x09, 0x00, 0x70, 0x0F, 0x0F, 0x0F, 0x00, 0x19}; //prefreq - pll0_divider
    send_serial(ftdi, prefreq1, 9);
    unsigned char prefreq2[9] = {0x51, 0x09, 0x00, 0x70, 0x0F, 0x0F, 0x0F, 0x00, 0x19}; //prefreq - pll0_divider
    send_serial(ftdi, prefreq2, 9);
    unsigned char freqbuf[9] = {0x51, 0x09, 0x00, 0x08, 0x40, 0xA0, 0x02, 0x25, 0x16}; //freqbuf - pll0_parameter
    send_serial(ftdi, freqbuf, 9);
    unsigned char freqbuf2[9] = {0x51, 0x09, 0x00, 0x08, 0x40, 0xA0, 0x02, 0x25, 0x16}; //freqbuf - pll0_parameter
    send_serial(ftdi, freqbuf2, 9);
    
}

