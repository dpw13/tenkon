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
