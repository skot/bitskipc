#ifndef BM1397_H_
#define BM1397_H_

#define TYPE_JOB 0x20
#define TYPE_CMD 0x40

#define GROUP_SINGLE 0x00
#define GROUP_ALL 0x10

#define CMD_JOB 0x01

#define CMD_SETADDRESS 0x00
#define CMD_WRITE 0x01
#define CMD_READ 0x02
#define CMD_INACTIVE 0x03

typedef enum {
  JOB_PACKET = 0, 
  CMD_PACKET = 1,
} packet_type_t;

struct __attribute__((__packed__)) job_packet {
  uint8_t job_id;
  uint8_t num_midstates;
  uint8_t starting_nonce[4];
  uint8_t nbits[4];
  uint8_t ntime[4];
  uint8_t merkle4[4];
  uint8_t midstates[4][32];
};

void send_read_address(struct ftdi_context *ftdi);
void send_init(struct ftdi_context *ftdi);
void send_work(struct ftdi_context *ftdi, struct job_packet *job);


#endif /* BM1397_H_ */