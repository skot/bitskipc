#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>    

#include "pretty.h"

void prettyHex(unsigned char * buf, int len) {
    int i;
    for (i = 0; i < len; i++) {
        if ((i > 0) && (buf[i] == 0xAA) && (buf[i+1] == 0x55))
            printf("\n");
        printf("%02X ", buf[i]);
    }
}

//flip byte order of a 32 bit integer
uint32_t flip32(uint32_t val) {
    uint32_t ret = 0;
    ret |= (val & 0xFF) << 24;
    ret |= (val & 0xFF00) << 8;
    ret |= (val & 0xFF0000) >> 8;
    ret |= (val & 0xFF000000) >> 24;
    return ret;
}


/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}