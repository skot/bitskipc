#ifndef SERIAL_MONITOR_H_
#define SERIAL_MONITOR_H_

void *serial_thread(void *arg);
struct ftdi_context * open_serial(void);

#endif // SERIAL_MONITOR_H_