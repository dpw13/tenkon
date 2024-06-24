#include <stdint.h>
#include <string.h>
#include "serial.h"
/* Use low-level printing to minimize complexity during exception handlers */
#include "print_utils.h"

#define INIT_VEC void __attribute__ ((section (".init")))

const char *EXCEPTION_NAMES[] = {
    "BusError",
    "AddressError",
    "IllegalInstruction",
    "ZeroDivide",
    "CHKInstruction",
    "TRAPVInstruction",
    "PrivilegeViolation",
    "Trace",
    "Line1010Emulator",
    "Line1111Emulator",
    "HardwareBreakpoint",
    "CoprocessorProtocolViolation",
    "FormatError",
    "UninitializedInterruptVector",
};

/*
 * __DefaultInterrupt handles all interrupt and exception vectors that have not
 * been overridden by the programmer.
 *
 * Unless handled more specifically, all exceptions and interrupts are masked
 * and the CPU stops executing.
 */
INIT_VEC __DefaultInterrupt(void) {
    uint16_t* fp = __builtin_frame_address(0) + 4;
    uint32_t tmp;
    uint8_t vec_idx;
    uint8_t frame_type;
    uint8_t frame_size; // size in 16-bit words

    vec_idx = ((fp[3] & 0x0FFF) - 8) >> 2;
    frame_type = fp[3] >> 12;

    // See 68030UM Table 8-6
    switch(frame_type) {
        case 0x2:
            frame_size = 6;
            break;
        case 0x9:
            frame_size = 10;
            break;
        case 0xA:
            frame_size = 16;
            break;
        case 0xB:
            frame_size = 46;
            break;
        default:
            frame_size = 4;
            break;
    }

    writeStringToSerial("! Caught exception ", 19);
    printU8(vec_idx);
    writeSerial(' ');
    if (vec_idx < sizeof(EXCEPTION_NAMES)/sizeof(char *)) {
        writeStringToSerial(EXCEPTION_NAMES[vec_idx], strlen(EXCEPTION_NAMES[vec_idx]));
    } else {
        writeStringToSerial("unknown", 8);
    }
    writeSerial('\n');

    for (int8_t word = 0; word <= frame_size; word++) {
        printU8(word);
        writeStringToSerial(": ", 3);
        printU16(fp[word]);
        writeSerial('\n');
    }

    writeStringToSerial("\nSR: 0x", 7);
    printU16(fp[0]);
    writeStringToSerial("\nPC: 0x", 7);
    tmp = fp[1] << 16 | fp[2];
    printU32(tmp);

    if (frame_type == 0xA || frame_type == 0xB) {
        /* Bus exceptions */
        writeStringToSerial("\nSSW: 0x", 8);
        printU16(fp[5]);
        writeStringToSerial("\nFault Addr: 0x", 15);
        tmp = fp[8] << 16 | fp[9];
        printU32(tmp);
        writeSerial('\n');
    }

    asm("stop #0x2700");
    while (1);
}