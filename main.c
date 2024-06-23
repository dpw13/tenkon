#include <stdio.h>
#include <stdint.h>
#include "serial.h"
#include "print_utils.h"

// Note that this is the ADDRESS of the start of the heap. This is a linker symbol,
// so its value is completely undefined.
extern uint32_t _heap_start;
#define SRAM_TEST_COUNT 16

void main(void) {
    volatile uint32_t* buf;
    uint32_t offset;
    uint8_t val;

    initializeSerial();
    writeStringToSerial("Init\n", 5);
    writeStringToSerial("Heap start = 0x", 15);
    printU32(_heap_start);
    writeStringToSerial(" at 0x", 6);
    printU32((uint32_t)&_heap_start);
    writeSerial('\n');

    buf = (uint32_t *)&_heap_start;

    for(offset = 0; offset < SRAM_TEST_COUNT; offset++) {
        writeSerial('0');
        writeSerial('x');
        printU32((uint32_t) buf);
        writeSerial('=');
        printU32(*buf++);
        writeSerial('\n');
    }
    while (1);
}
