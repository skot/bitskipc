#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "serial_monitor.h"
#include "pretty.h"

#define SLEEP_TIME 20


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
    msleep(SLEEP_TIME);
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
    msleep(SLEEP_TIME);
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

void send_work(struct ftdi_context *ftdi) {
    //this should find nonce 258a8b34 @ 50
    uint8_t work1[150] = {0x21, 0x96, 0x50, 0x04, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xE0, 0x05, 0x17, 0x24, 0x27, 0x36, 0x64, 0xF5, 0x63, 0x54, 0xDA, 0x33, 0xE2, 0xDE, 0x8F, 0xFC, 0xDD, 0x48, 0x96, 0xE1, 0x36, 0xD7, 0x03, 0x5C, 0xBB, 0x5F, 0xA3, 0xFD, 0x5F, 0x68, 0x39, 0xAA, 0xA4, 0xBE, 0x10, 0x9C, 0x7E, 0x00, 0x78, 0x4E, 0x69, 0x34, 0xAC, 0x84, 0x05, 0x65, 0xAE, 0x32, 0x58, 0x09, 0xBB, 0xEA, 0x44, 0x6D, 0x61, 0x57, 0xF2, 0x61, 0xBE, 0x58, 0x33, 0xFA, 0xA8, 0x1D, 0x9A, 0x16, 0xBF, 0xE0, 0x82, 0x64, 0x37, 0x91, 0x15, 0xB6, 0x32, 0x93, 0xC4, 0x83, 0x42, 0xB2, 0xE6, 0x63, 0x96, 0xE0, 0x25, 0x02, 0x9E, 0x01, 0x76, 0xD9, 0x24, 0x0F, 0xD3, 0x57, 0x27, 0x38, 0xE2, 0x65, 0xDD, 0xCD, 0xBD, 0x01, 0xE0, 0x61, 0xFB, 0x57, 0x5D, 0xD6, 0xAB, 0xAE, 0xFD, 0x6B, 0x5F, 0x77, 0x74, 0x5C, 0x64, 0x2C, 0xF3, 0x34, 0x2F, 0x82, 0xB3, 0xCC, 0xC1, 0x2D, 0x84, 0xDD, 0xCB, 0x10, 0xDE, 0x5E, 0xE0, 0xCD, 0x9C, 0x5B, 0x65, 0x92, 0xBB, 0x2B, 0x26};
    send_serial(ftdi, work1, 15);
}