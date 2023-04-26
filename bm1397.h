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

void send_read_address(struct ftdi_context *ftdi);
void send_init(struct ftdi_context *ftdi);
void send_work(struct ftdi_context *ftdi);


#endif /* BM1397_H_ */