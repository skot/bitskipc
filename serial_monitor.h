#ifndef SERIAL_MONITOR_H_
#define SERIAL_MONITOR_H_

#include <ftdi.h>

#define CHUNK_SIZE 1024

int16_t serial_rx(struct ftdi_context *ftdi, uint8_t * buf);
struct ftdi_context * open_serial(void);
int write_data(struct ftdi_context *, const unsigned char *, int);

#endif // SERIAL_MONITOR_H_