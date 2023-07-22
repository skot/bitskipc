#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "serial_monitor.h"
#include "pretty.h"
#include "bm1366.h"
#include "crc.h"

#define SLEEP_TIME 20

//reset the BM1397 via the RTS line
void reset_BM1366(struct ftdi_context *ftdi) {
    int ret;

    //set the RTS line low
    ret = ftdi_setrts(ftdi, 1);
    if (ret < 0) {
        fprintf(stderr, "unable to set RTS: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
    }

    //sleep for 100ms
    msleep(100);

    //set the RTS line high
    ret = ftdi_setrts(ftdi, 0);
    if (ret < 0) {
        fprintf(stderr, "unable to set RTS: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
    }

    //sleep for 100ms
    msleep(100);

}

//parse job/nonce response
void parse_job_response(unsigned char *buf, int len) {
    struct nonce_response * nonce;

    //get the response into the nonce struct
    //memcpy((void *)&nonce, buf, len);
    nonce = (struct nonce_response *)buf;

    printf("nonce: %08X @ %02X\n", flip32(nonce->nonce), nonce->job_id);

}

void parse_cmd_packet(unsigned char *buf, int len) {
    printf("cmd packet\n");
}

//split the response packet into individual packets
void split_response(unsigned char *buf, int len) {
    int i;
    int packet_len;
    int packet_start = 0;

    //split the response into individual packets
    for (i = 1; i < len; i++) {
        if ((buf[i] == 0xAA) && (buf[i+1] == 0x55)) {
            packet_len = i - packet_start;
            parse_packet(buf+packet_start, packet_len);
            packet_start = i;
        }
    }

    //parse the last packet
    packet_len = i - packet_start;
    parse_packet(buf+packet_start, packet_len);
}


//parse incoming packets
void parse_packet(unsigned char *buf, int len) {
    response_type_t response_type;

    //debug the packet
    printf("<-");
    prettyHex(buf, len);
    printf("\n");


    //determine response type
    if (buf[len-1] & RESPONSE_JOB) {
        response_type = JOB_RESP;
        parse_job_response(buf, len);
    } else {
        response_type = CMD_RESP;
        parse_cmd_packet(buf, len);
    }

}

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

void send_first_thing(struct ftdi_context *ftdi) {

    unsigned char read_address[6] = {0x00, 0xA4, 0x90, 0x00, 0xFF, 0xFF};
    //send serial data
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), read_address, 6);
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

    unsigned char init[6] = {0x00, 0xA8, 0x00, 0x07, 0x00, 0x00}; //init1
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), init, 6);

    unsigned char init2[6] = {0x00, 0x18, 0xFF, 0x0F, 0xC1, 0x00}; //init2
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), init2, 6);

    unsigned char init3[9] = {0x00, 0x00}; //init3
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_INACTIVE), init3, 2);

    unsigned char init4[9] = {0x00, 0x00}; //init4
    send_BM1397(ftdi, (TYPE_CMD | GROUP_SINGLE | CMD_SETADDRESS), init4, 2);

    unsigned char init5[9] = {0x00, 0x3C, 0x80, 0x00, 0x85, 0x40}; //init5
    send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), init5, 6);

    // unsigned char set_ticket[9] = {0x00, 0x14, 0x00, 0x00, 0x00, 0x00}; //set_ticket - ticket_mask
    // send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), set_ticket, 6);

    // unsigned char init5[9] = {0x00, 0x68, 0xC0, 0x70, 0x01, 0x11}; //init5 - pll3_parameter
    // send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), init5, 6);

    // unsigned char init5_2[9] = {0x00, 0x68, 0xC0, 0x70, 0x01, 0x11}; //init5_2 - pll3_parameter
    // send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), init5_2, 6);

    // unsigned char init6[9] = {0x00, 0x28, 0x06, 0x00, 0x00, 0x0F}; //init6 - fast_uart_configuration
    // send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), init6, 6);

    // unsigned char baudrate[9] = {0x00, 0x18, 0x00, 0x00, 0x7A, 0x31}; //baudrate - misc_control
    // send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), baudrate, 6);

    // unsigned char prefreq1[9] = {0x00, 0x70, 0x0F, 0x0F, 0x0F, 0x00}; //prefreq - pll0_divider
    // send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), prefreq1, 6);

    // unsigned char freqbuf[9] = {0x00, 0x08, 0x40, 0xA0, 0x02, 0x25}; //freqbuf - pll0_parameter
    // send_BM1397(ftdi, (TYPE_CMD | GROUP_ALL | CMD_WRITE), freqbuf, 6);
    
}

void send_work(struct ftdi_context *ftdi, struct job_packet *job) {
  
    send_BM1397(ftdi, (TYPE_JOB | GROUP_SINGLE | CMD_WRITE), (uint8_t*)job, sizeof(struct job_packet));

}

void send_work_manual(struct ftdi_context *ftdi) {
    unsigned char faux_job[] = {0x28, 0x01, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x95, 0x24, 0x19, 0xE0, 0x16, 0xAF, 0x64, 0xF3, 0xC0, 0x09, 0x10, 0xCB, 0xDA, 0xF2, 0x3F, 0x0F, 0xFF, 0xDF, 0x3F, 0x7B, 0x42, 0xAC, 0xC9, 0x8D, 0x70, 0xED, 0x9B, 0xBF, 0xBA, 0x1B, 0x1D, 0x44, 0x74, 0xED, 0xE0, 0x32, 0x28, 0x85, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x65, 0x90, 0x49, 0x10, 0x58, 0x57, 0xDD, 0xA2, 0x47, 0x8A, 0x5E, 0x7C, 0xCC, 0xFE, 0x03, 0x6F, 0xE8, 0xBE, 0x8E, 0xF8, 0xC2, 0x57, 0x21, 0xD4, 0x00, 0x00, 0x00, 0x20}; //init5
    send_BM1397(ftdi, (TYPE_JOB | GROUP_SINGLE | CMD_WRITE), faux_job, sizeof(faux_job));
}