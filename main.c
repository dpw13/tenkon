#include <stdio.h>

void __DefaultInterrupt(void) {
    irq: goto irq;
}

int _start(void) {
    label1: goto label1;
    return 0;
}