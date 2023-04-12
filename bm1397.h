#ifndef BM1397_H_
#define BM1397_H_

void send_chippy(struct ftdi_context *ftdi);
void send_init(struct ftdi_context *ftdi);
void send_work(struct ftdi_context *ftdi);

#endif /* BM1397_H_ */