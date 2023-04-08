#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "pretty.h"

void prettyHex(unsigned char * buf, int len) {
    int i;
    for (i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
}