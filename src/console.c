#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "serial.h"
#include "asm.h"
#include "s1d13506.h"
#include "test.h"

inline void memWriteByte(const uint8_t b, const uintptr_t addr) {
    *(uint8_t *)addr = b;
}

inline void memWriteWord(const uint16_t b, const uintptr_t addr) {
    *(uint16_t *)addr = b;
}

inline void memWriteLWord(const uint32_t b, const uintptr_t addr) {
    *(uint32_t *)addr = b;
}

inline uint8_t memReadByte(const uintptr_t addr) {
   return *(uint8_t *)addr;
}

inline uint16_t memReadWord(const uintptr_t addr) {
   return *(uint16_t *)addr;
}

inline uint32_t memReadLWord(const uintptr_t addr) {
   return *(uint32_t *)addr;
}

void run(const uintptr_t addr, const uint8_t debug) {
    if (debug)
        trace_flow();
    // Do not do this. Seriously under no circumstances
    // should you copy this code into any other program!
    // Should you decide to do so your descendants will
    // curse you and the day you did for generations
    ((void (*)(void))addr)();
}

void loadBytes(uintptr_t addr) {
    uint8_t value = 0;

    while(scanf("%02hhx", &value)) {
        memWriteByte(value, addr);
        addr += sizeof(value);
    }
}

void loadWords(uintptr_t addr, const uint8_t shift) {
    uint16_t value = 0;

    while(scanf("%04hx", &value)) {
        memWriteWord(rol16(value, shift), addr);
        addr += sizeof(value);
    }
}

void loadLWords(uintptr_t addr) {
    uint32_t value = 0;
    uint32_t sum = 0;
    uint8_t nibble_count = 0;
    uint32_t *buf = (uint32_t *)addr;
    char c;

    /* Clear any existing serial errors */
    serialGetError();

    /* When loading long words use a faster implementation so
     * we can keep up with line rate when transferring large
     * applications. */

    while (1) {
        c = readSerial();
        if (isdigit(c)) {
            value = (value << 4) | (c - '0');
            nibble_count++;
        } else if (isxdigit(c)) {
            value = (value << 4) | (0xa + c - (c >= 'a' ? 'a' : 'A'));
            nibble_count++;
        } else if (isspace(c)) {
            if (nibble_count > 0) {
                /* If we've recorded any data at all, write to memory
                 * Note that we don't validate that we got 8 nibbles.
                 */
                *buf++ = value;
                sum += value;
                nibble_count = 0;
            }
        } else {
            break;
        }
    }

    if ((c = serialGetError())) {
        printf("Encountered serial error: %02x", c);
    }

    printf("Received %d bytes\n", (uintptr_t)buf - addr);
}

void showBytes(const uintptr_t addr, const int size) {
    const uintptr_t target = addr + size;
    uintptr_t ptr = addr & ~0 << 4;

    printf("         | 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15\n");
    while(ptr < target) {
        printf("%08x | ", ptr);
        for(int j = 0; j < 16; j++) {
            if (ptr >= addr && ptr < target) {
                printf("%02hhx ", memReadByte(ptr));
            } else {
                printf("   ");
            }
            ptr += sizeof(uint8_t);
        }
        printf("\n");
    }
}

void showWords(const uintptr_t addr, const int size, const int shift) {
    const uintptr_t target = addr + size;
    uintptr_t ptr = addr & ~0 << 4;

    printf("         | 00   02   04   06   08   10   12   14\n");
    while(ptr < target) {
        printf("%08x | ", ptr);
        for(int j = 0; j < 8; j++) {
            if (ptr >= addr && ptr < target) {
                uint16_t val = memReadWord(ptr);
                val = ror16(val, shift);
                printf("%04hx ", val);
            } else {
                printf("     ");
            }
            ptr += sizeof(uint16_t);
        }
        printf("\n");
    }
}

void showLWords(const uintptr_t addr, const int size) {
    const uintptr_t target = addr + size;
    uintptr_t ptr = addr & ~0 << 4;

    printf("         | 00       04       08       12\n");
    while(addr < target) {
        printf("%08x | ", ptr);
        for(int j = 0; j < 4; j++) {
            if (ptr >= addr && ptr < target) {
                printf("%08x ", memReadLWord(ptr));
            } else {
                printf("         ");
            }
            ptr += sizeof(uint32_t);
        }
        printf("\n");
    }
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
    // Remember that these are symbols with addresses but no allocated
    // contents.
    extern uint8_t _text_start;
    extern uint8_t _data_start;
    extern uint8_t _heap_start;
    extern uint8_t _heap_end;
    extern uint8_t _stack_start;
    extern uint8_t _stack_end;

    asm volatile ("movew %%sr, %0"
        : "=r" (statusReg));
    asm volatile ("movel %%fp, %0"
        : "=r" (framePtr));
    asm volatile ("movel %%sp, %0"
        : "=r" (stackPtr));

    printf("Caller: %p\n", retAddr);
    printf("FP: %p\n", framePtr);
    printf("SP: %p\n", stackPtr);
    printf("SR: 0x%04hx\n\n", statusReg);

    printf("Code start: %p\n", &_text_start);
    printf("Data start: %p\n", &_data_start);
    printf("Heap: %p:%p\n", &_heap_start, &_heap_end);
    printf("Stack: %p:%p\n", &_stack_start, &_stack_end);
}

static inline void initDRAM(void) {
    /* Configure DRAM */
    *(uint8_t *)0xF8EBE583 = 0;
	/* Delay. Must be at least 60 ms */
	for (uint32_t i = 0; i < 100000; i++)
		asm("nop");
}

void runUboot(void) {
    /* U-boot runs from SRAM and does its own RAM init */
    /* Jump to u-boot entry */
    run(0xFFF08400, 0);
}

void consoleLoop(void) {
    char cmd;
    uintptr_t addr = 0;
    uint32_t size = 0;
    uint16_t word = 0;
    int line = 0;

    while(1) {  //run forever
        printf("%d >", line);
        fflush(stdout);
        line++;
        scanf(" %c", &cmd);
        switch(cmd) {
            case 'L':  //Load
            case 'l':
                if (!scanf(".%c", &cmd)) {
                    cmd = 'B';
                }
                scanf("%x", &addr);
                switch(cmd) {
                    default:
                        printf("Don't understand format '%c', using bytes\n", cmd);
                        // Fallthrough
                    case 'b':
                    case 'B':
                        loadBytes(addr);
                        break;
                    case 'w':
                    case 'W':
                        loadWords(addr, 0);
                        break;
                    case 'e':
                    case 'E':
                        /* Ethernet shift */
                        loadWords(addr, 1);
                        break;
                    case 'l':
                    case 'L':
                        loadLWords(addr);
                        break;
                }
                scanf("%c", &cmd); //Throw away the extra Z
                break;
            case 'P':  //Peek
            case 'p':
                if (!scanf(".%c", &cmd)) {
                    cmd = 'B';
                }
                scanf("%x", &addr);
                scanf("%d", &size);
                switch(cmd) {
                    default:
                        printf("Don't understand format '%c', using bytes\n", cmd);
                        // Fallthrough
                    case 'b':
                    case 'B':
                        showBytes(addr, size);
                        break;
                    case 'w':
                    case 'W':
                        showWords(addr, size, 0);
                        break;
                    case 'e':
                    case 'E':
                        /* Ethernet shift */
                        showWords(addr, size, 1);
                        break;
                    case 'l':
                    case 'L':
                        showLWords(addr, size);
                        break;
                }
                break;
            case 'R':  //Run
            case 'r':
                if (!scanf(".%c", &cmd)) {
                    cmd = ' ';
                }
                scanf("%x", &addr);
                run(addr, cmd == 'd');
                break;
            case 'M': //Memtest
            case 'm':
                if (!scanf(".%c", &cmd)) {
                    // Default to random data
                    cmd = 'R';
                }
                scanf("%x", &addr);
                scanf("%d", &size);
                memTest(addr, size, cmd);
                break;
            case 'T': //io test
            case 't':
                scanf("%d", &size);
                initDRAM();
                ioTest(size);
                break;
            case 's': //Set status register
            case 'S':
                scanf("%hx", &word);
                set_status(word);
                break;
            case 'i': //Context info
            case 'I':
                contextInfo();
                break;
            case 'd': //Enable trace
            case 'D':
                trace_flow();
                break;
            case 'u':
            case 'U':
                runUboot();
                break;
            case 'x':
            case 'X':
                scanf("%x", &addr);
                s1d13506_init((uint8_t *)addr);
                break;
            default:
                printf("Unrecognized Command: %c\n", cmd);
                break;
        }
    }
}

int main(void) {
    // This should go before any other initialization so we can print exceptions.
    initializeSerial();

    // Initialize DRAM
    // TODO: code out the bitfields used here instead of this magic number.
    //*((uint8_t *)0xF8EBE583) = 0;

    writeStringToSerial("Init\n", 5);

    printf("Welcome to the Tenkon Basic Console!\n");
    printf("Commands:\n");
    printf("L addr      - Loads bytes represented in Hex starting at addr.\n");
    printf("              Continues to load bytes until you send Z.\n");
    printf("  L.b addr  - Loads by byte (default).\n");
    printf("  L.w addr  - Loads by 16-bit word.\n");
    printf("  L.l addr  - Loads by 32-bit long word\n");
    printf("P addr size - Peeks at size bytes at addr.\n");
    printf("  P.b a s   - Peeks by byte (default).\n");
    printf("  P.w a s   - Peeks by 16-bit word.\n");
    printf("  P.l a s   - Peeks by 32-bit long word\n");
    printf("R addr      - Calls subroutine located at addr\n");
    printf("  R.d addr  - Enables control flow tracing before calling subroutine\n");
    printf("M addr size - Test size bytes of memory starting at addr\n");
    printf("T iteration - Test i/o accesses\n");
    printf("S value     - Set status register\n");
    printf("I           - Print info about the current execution context\n");
    printf("D           - Trace control flow\n");
    printf("U           - Initialize DRAM and run u-boot\n");
    consoleLoop();
    return 0;
}
