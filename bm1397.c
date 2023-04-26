#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "serial_monitor.h"
#include "pretty.h"
#include "bm1397.h"
#include "crc.h"

#define SLEEP_TIME 20

int send_serial(struct ftdi_context *ftdi, unsigned char *buf, int len) {
    int ret;

    printf("->");
    prettyHex(buf, len);
    printf("\n");
    
    //write the data to the serial port
    ret = ftdi_write_data(ftdi, buf, len);

    if (ret < 0) {
        fprintf(stderr, "ftdi_write_data failed, error %d (%s)", ret, ftdi_get_error_string(ftdi));
    }
    msleep(SLEEP_TIME);
    return ret;
}

/// @brief 
/// @param ftdi 
/// @param header 
/// @param data 
/// @param len 
void send_BM1397(struct ftdi_context *ftdi, uint8_t header, uint8_t * data, uint8_t data_len) {
    packet_type_t packet_type = (header & TYPE_JOB) ? JOB_PACKET : CMD_PACKET;
    uint8_t total_length = (packet_type == JOB_PACKET) ? (data_len+6) : (data_len+5);

    //allocate memory for buffer
    unsigned char *buf = malloc(total_length);

    //add the preamble
    buf[0] = 0x55;
    buf[1] = 0xAA;

    //add the header field
    buf[2] = header;

    //add the length field
    buf[3] = (packet_type == JOB_PACKET) ? (data_len+4) : (data_len+3);

    //add the data
    memcpy(buf+4, data, data_len);

    //add the correct crc type
    if (packet_type == JOB_PACKET) {
        uint16_t crc16_total = crc16_false(buf+2, data_len+2);
        buf[4+data_len] = (crc16_total >> 8) & 0xFF;
        buf[5+data_len] = crc16_total & 0xFF;
    } else {
        buf[4+data_len] = crc5(buf+2, data_len+2);
    }

    //send serial data
    send_serial(ftdi, buf, total_length);

    free(buf);
}

void send_read_address(struct ftdi_context *ftdi) {

    unsigned char read_address[2] = {0x00, 0x00};
    //send serial data
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_READ), read_address, 2);
}

void send_chain_inactive(struct ftdi_context *ftdi) {

    unsigned char read_address[2] = {0x00, 0x00};
    //send serial data
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_INACTIVE), read_address, 2);
}

void set_chip_address(struct ftdi_context *ftdi, uint8_t chipAddr) {

    unsigned char read_address[2] = {chipAddr, 0x00};
    //send serial data
    send_BM1397(ftdi, (TYPE_CMD | GROUP_SINGLE | CMD_SETADDRESS), read_address, 2);
}



void send_init(struct ftdi_context *ftdi) {

    //send serial data
    msleep(SLEEP_TIME);
    send_chain_inactive(ftdi);

    set_chip_address(ftdi, 0x00);

    unsigned char init[6] = {0x00, 0x80, 0x00, 0x00, 0x00, 0x00}; //init1 - clock_order_control0
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), init, 6);

    unsigned char init2[6] = {0x00, 0x84, 0x00, 0x00, 0x00, 0x00}; //init2 - clock_order_control1
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), init2, 6);

    unsigned char init3[9] = {0x00, 0x20, 0x00, 0x00, 0x00, 0x01}; //init3 - ordered_clock_enable
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), init3, 6);

    unsigned char init4[9] = {0x00, 0x3C, 0x80, 0x00, 0x80, 0x74}; //init4 - init_4_?
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), init4, 6);

    unsigned char set_ticket[9] = {0x00, 0x14, 0x00, 0x00, 0x00, 0x00}; //set_ticket - ticket_mask
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), set_ticket, 6);

    unsigned char init5[9] = {0x00, 0x68, 0xC0, 0x70, 0x01, 0x11}; //init5 - pll3_parameter
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), init5, 6);

    unsigned char init5_2[9] = {0x00, 0x68, 0xC0, 0x70, 0x01, 0x11}; //init5_2 - pll3_parameter
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), init5_2, 6);

    unsigned char init6[9] = {0x00, 0x28, 0x06, 0x00, 0x00, 0x0F}; //init6 - fast_uart_configuration
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), init6, 6);

    unsigned char baudrate[9] = {0x00, 0x18, 0x00, 0x00, 0x7A, 0x31}; //baudrate - misc_control
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), baudrate, 6);

    unsigned char prefreq1[9] = {0x00, 0x70, 0x0F, 0x0F, 0x0F, 0x00}; //prefreq - pll0_divider
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), prefreq1, 6);

    unsigned char prefreq2[9] = {0x00, 0x70, 0x0F, 0x0F, 0x0F, 0x00}; //prefreq - pll0_divider
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), prefreq2, 6);

    unsigned char freqbuf[9] = {0x00, 0x08, 0x40, 0xA0, 0x02, 0x25}; //freqbuf - pll0_parameter
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), freqbuf, 6);

    unsigned char freqbuf2[9] = {0x00, 0x08, 0x40, 0xA0, 0x02, 0x25}; //freqbuf - pll0_parameter
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), freqbuf2, 6);
    
}

void send_work(struct ftdi_context *ftdi) {
    //this should find nonce 258a8b34 @ 50
    uint8_t work1[146] = {0x50, 0x04, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xE0, 0x05, 0x17, 0x24, 0x27, 0x36, 0x64, 0xF5, 0x63, 0x54, 0xDA, 0x33, 0xE2, 0xDE, 0x8F, 0xFC, 0xDD, 0x48, 0x96, 0xE1, 0x36, 0xD7, 0x03, 0x5C, 0xBB, 0x5F, 0xA3, 0xFD, 0x5F, 0x68, 0x39, 0xAA, 0xA4, 0xBE, 0x10, 0x9C, 0x7E, 0x00, 0x78, 0x4E, 0x69, 0x34, 0xAC, 0x84, 0x05, 0x65, 0xAE, 0x32, 0x58, 0x09, 0xBB, 0xEA, 0x44, 0x6D, 0x61, 0x57, 0xF2, 0x61, 0xBE, 0x58, 0x33, 0xFA, 0xA8, 0x1D, 0x9A, 0x16, 0xBF, 0xE0, 0x82, 0x64, 0x37, 0x91, 0x15, 0xB6, 0x32, 0x93, 0xC4, 0x83, 0x42, 0xB2, 0xE6, 0x63, 0x96, 0xE0, 0x25, 0x02, 0x9E, 0x01, 0x76, 0xD9, 0x24, 0x0F, 0xD3, 0x57, 0x27, 0x38, 0xE2, 0x65, 0xDD, 0xCD, 0xBD, 0x01, 0xE0, 0x61, 0xFB, 0x57, 0x5D, 0xD6, 0xAB, 0xAE, 0xFD, 0x6B, 0x5F, 0x77, 0x74, 0x5C, 0x64, 0x2C, 0xF3, 0x34, 0x2F, 0x82, 0xB3, 0xCC, 0xC1, 0x2D, 0x84, 0xDD, 0xCB, 0x10, 0xDE, 0x5E, 0xE0, 0xCD, 0x9C, 0x5B, 0x65, 0x92, 0xBB};
    send_BM1397(ftdi, (TYPE_JOB | GROUP_SINGLE | CMD_WRITE), work1, sizeof(work1));
    //send_serial(ftdi, work1, 150);
}