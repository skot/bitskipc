#ifndef BM1366_H_
#define BM1366_H_

#include <ftdi.h>

#define TYPE_JOB 0x20
#define TYPE_CMD 0x40

#define GROUP_SINGLE 0x00
#define GROUP_ALL 0x10

#define CMD_JOB 0x01

#define CMD_SETADDRESS 0x00
#define CMD_WRITE 0x01
#define CMD_READ 0x02
#define CMD_INACTIVE 0x03

#define RESPONSE_CMD 0x00
#define RESPONSE_JOB 0x80
#define CRC5_MASK 0x1F

typedef enum {
  JOB_PACKET = 0, 
  CMD_PACKET = 1,
} packet_type_t;

typedef enum {
  JOB_RESP = 0, 
  CMD_RESP = 1,
} response_type_t;

struct __attribute__((__packed__)) job_packet {
  uint8_t job_id;
  uint8_t num_midstates;
  uint8_t starting_nonce[4];
  uint8_t nbits[4];
  uint8_t ntime[4];
  uint8_t merkle4[4];
  uint8_t midstates[4][32];
};

struct __attribute__((__packed__)) nonce_response {
    uint8_t preamble[2];
    uint32_t nonce;
    uint8_t midstate_num;
    uint8_t job_id;
    uint8_t crc;
};


void send_first_thing(struct ftdi_context *ftdi);
void send_read_address(struct ftdi_context *ftdi);
void send_init(struct ftdi_context *ftdi);
void send_work(struct ftdi_context *ftdi, struct job_packet *job);
void parse_packet(unsigned char *buf, int len);
void split_response(unsigned char *buf, int len);
void reset_BM1366(struct ftdi_context *ftdi);
void send_work_manual(struct ftdi_context *ftdi);


#endif /* BM1366_H_ */