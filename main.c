#include <stdio.h>
#include <stdint.h>
#include "serial.h"

void printNibble(const uint8_t val) {
    if (val >= 10) {
        writeSerial('A' + val - 10);
    } else {
        writeSerial('0' + val);
    }
}

void printU8(const uint8_t val) {
    printNibble(val >> 4);
    printNibble(val & 0x0F);
}

void printU16(const uint16_t val) {
    printU8(val >> 8);
    printU8(val & 0xFF);
}

void printU32(const uint32_t val) {
    printU8((val >> 24) & 0xFF);
    printU8((val >> 16) & 0xFF);
    printU8((val >> 8) & 0xFF);
    printU8(val & 0xFF);
}

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
