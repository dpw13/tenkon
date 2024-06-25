#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "serial.h"

void memWrite(const char b, const int addr) {
    *(char *)addr = b;
}

char memRead(const int addr) {
   return *(char *)addr;
}

void run(const int addr) {
    // Do not do this. Seriously under no circumstances
    // should you copy this code into any other program!
    // Should you decide to do so your descendants will
    // curse you and the day you did for generations
    ((void (*)(void))addr)();
}

void loadData(int addr) {
    char value = 0;
    while(scanf("%02hhx", &value)) {
        memWrite(value, addr);
        addr++;
    }
}

void showData(int addr, const int size) {
    printf("         | 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15\n");
    const int target = addr + size;
    addr = (addr / 16) * 16;
    while(addr < target) {
        printf("%08x | ", addr);
        for(int j = 0; j < 16; j++) {
            printf("%02hhx ", memRead(addr));
            addr++;
        }
        printf("\n");
    }
}

/* Note that this overwrites values in memory in the specified window, so
 * avoid calling this with a window that overlaps the current stack.
 */
void memTest(const int addr, const int size) {
    uint32_t *buf;
    uint32_t offset;
    uint32_t errCount;

    /* Seed PRNG so we always get the same sequence */
    srand(0);
    buf = (uint32_t *)addr;
    for (offset = 0; offset < size; offset += sizeof(uint32_t)) {
        *buf++ = rand();
    }

    /* Reset PRNG sequence */
    srand(0);
    buf = (uint32_t *)addr;
    errCount = 0;
    for (offset = 0; offset < size; offset += sizeof(uint32_t)) {
        uint32_t val = rand();
        if (*buf != val) {
            /* Only print first error */
            if (errCount == 0) {
                printf("Mismatch at %p: expected %08x but read %08x. Supressing further error messages.\n", buf, val, *buf);
            }
            errCount += 1;
        }
    }

    printf("Found %d errors\n", errCount);
}

/* Print out some minimal info about the current execution context
 * Useful for verifying supervisor mode, interrupt mask, and current
 * code and stack locations.
 */
void contextInfo(void) {
    uint16_t statusReg;
    void *retAddr = __builtin_return_address(0);
    void *framePtr;
    void *stackPtr;

    asm volatile ("movew %%sr, %0"
        : "=r" (statusReg));
    asm volatile ("movel %%fp, %0"
        : "=r" (framePtr));
    asm volatile ("movel %%sp, %0"
        : "=r" (stackPtr));
    
    printf("Caller: %p\n", retAddr);
    printf("FP: %p\n", framePtr);
    printf("SP: %p\n", stackPtr);
    printf("SR: %04hx\n", statusReg);
}

void consoleLoop(void) {
    char cmd;
    int addr = 0;
    int size = 0;
    int line = 0;

    while(1) {  //run forever
        printf("%d >", line);
        fflush(stdout);
        line++;
        scanf(" %c", &cmd);
        switch(cmd) {
            case 'L':  //Load
            case 'l':
                scanf("%x", &addr);
                loadData(addr);
                scanf("%c", &cmd); //Throw away the extra Z
                break;
            case 'P':  //Peek
            case 'p':
                scanf("%x", &addr);
                scanf("%d", &size);
                showData(addr, size);
                break;
            case 'R':  //Run
            case 'r':
                scanf("%x", &addr);
                run(addr);
                break;
            case 'M': //Memtest
            case 'm':
                scanf("%x", &addr);
                scanf("%d", &size);
                memTest(addr, size);
                break;
            case 'i': //Context info
            case 'I':
                contextInfo();
                break;
            default:
                printf("Unrecognized Command: %c\n", cmd);
                break;
        }
    }
}

int main(void) {
    initializeSerial();
    writeStringToSerial("Init\n", 5);

    printf("Welcome to the Tenkon Basic Console!\n");
    printf("Commands:\n");
    printf("L addr      - Loads bytes represented in Hex starting at addr.\n");
    printf("              Continues to load bytes until you send Z.\n");
    printf("P addr size - Peeks at size bytes at addr.\n");
    printf("R addr      - Calls subroutine located at addr\n");
    printf("M addr size - Test size bytes of memory starting at addr\n");
    printf("I           - Print info about the current execution context\n");
    consoleLoop();
    return 0;
}
