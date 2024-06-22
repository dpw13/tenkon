#include <stdio.h>
#include <stdint.h>

#if 1

#include "serial.h"

void main(void) {
    initializeSerial();

    while (1) {
        writeCharToSerial(0x5A);
    }
}

#else

/* We force -O0 for this function because `count` gets optimized away without being used. */
int __attribute__((optimize("O0"))) main(void) {
    int count = 0;
    while (1) {
        printf("Count: %d\n", count++);
    }
    return 0;
}

#endif