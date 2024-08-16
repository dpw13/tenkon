#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static uint32_t getSequenceWord(const uintptr_t addr, const uint8_t mode) {
    switch (mode) {
        case '0':
            return 0x00000000;
        case '1':
            return 0xFFFFFFFF;
        case 'A': // alternating 0,1
            return 0xAAAAAAAA;
        case '5': // alternating 1,0
            return 0x55555555;
        case 's':
        case 'S': // self-address
            return addr;
        case 'r':
        case 'R':
        default:
            return rand();
    }
}

/* Note that this overwrites values in memory in the specified window, so
 * avoid calling this with a window that overlaps the current stack.
 */
void memTest(const uintptr_t addr, const uintptr_t size, const uint8_t mode) {
    uint32_t *buf;
    uint32_t offset;
    uint32_t errCount;

    /* Seed PRNG so we always get the same sequence */
    srand(0);
    buf = (uint32_t *)addr;
    for (offset = 0; offset < size; offset += sizeof(uint32_t)) {
        *buf++ = getSequenceWord(addr + offset, mode);
    }

    /* Reset PRNG sequence */
    srand(0);
    buf = (uint32_t *)addr;
    errCount = 0;
    for (offset = 0; offset < size; offset += sizeof(uint32_t)) {
        uint32_t val = getSequenceWord(addr + offset, mode);
        if (*buf != val) {
            /* Only print first error */
            if (errCount == 0) {
                printf("Mismatch at %p: expected %08x but read %08x. Supressing further error messages.\n", buf, val, *buf);
            }
            errCount += 1;
        }
        *buf++ = ~val;
    }

    /* If the mode isn't random check the inverted values */
    if(mode != 'r' && mode != 'R') {
        printf("Checking Inverted Pattern\n");
        buf = (uint32_t *)(addr + size);
        for(offset = size; offset > 0; offset -= sizeof(uint32_t)) {
            uint32_t val = ~ getSequenceWord(addr + offset, mode);
            if (*buf != val) {
                if (errCount == 0) {
                    printf("Mismatch at %p: expected %08x but read %08x. Supressing further error messages.\n", buf, val, *buf);
                }
                errCount += 1;
            }
            buf--;
        }
    }

    printf("Found %d errors\n", errCount);
}

/* Interleave accesses to different cards to test bus accesses.
 */

/* 8-bit card: "user flag/status" register of DUART = 0xFE00000C or 0xFE40000C */
#define TEST_ADDR_8 ((volatile uint8_t *)0xFE00000C)
/* 16-bit card: BNDRY register of RTL8019 = 0xFD800006 (data in top 7 bits of 16-bit access) */
#define TEST_ADDR_16 ((volatile uint16_t *)0xFD800006)
/* DRAM card: Anything in the window */
#define TEST_ADDR_32 ((volatile uint32_t *)0x04F0F0F0)
/* SRAM: anything unused by console. Use interrupt vector at 0x3F8 */
#define TEST_ADDR_LCL ((volatile uint32_t *)0x000003F8)

#define COMPARE(x, y, msg, i) \
    if(x != y) { printf(msg " mismatch at %d: %x != %x\n", i, x, y); }

#define COMPARE_ALL(msg, i) do { \
    COMPARE(val_bak[0], val_8, msg " 8-bit", i) \
    COMPARE(val_bak[1], val_16, msg " 16-bit", i) \
    COMPARE(val_bak[2], val_32, msg " DRAM", i) \
    COMPARE(val_bak[3], val_lcl, msg " SRAM", i) \
} while(0)

#define RESTORE_ALL() do { \
    val_8 = val_bak[0]; \
    val_16 = val_bak[1]; \
    val_32 = val_bak[2]; \
    val_lcl = val_bak[3]; \
} while(0)

#define INVERT_ALL() do { \
    val_bak[0] = ~val_bak[0] & 0xFF; \
    val_bak[1] = (~val_bak[1] & 0xFFFF) | 0x01FE; \
    val_bak[2] = ~val_bak[2]; \
    val_bak[3] = ~val_bak[3]; \
} while(0)

#define ORDER_A "8/16/DRAM/SRAM"
#define ORDER_B "SRAM/DRAM/16/8"
#define ORDER_C "SRAM/8/DRAM/16"
#define ORDER_D "16/DRAM/8/SRAM"

#define WRITE_A() do { \
    *TEST_ADDR_8 = val_8; \
    *TEST_ADDR_16 = val_16; \
    *TEST_ADDR_32 = val_32; \
    *TEST_ADDR_LCL = val_lcl; \
} while(0)

#define WRITE_B() do { \
    *TEST_ADDR_LCL = val_lcl; \
    *TEST_ADDR_32 = val_32; \
    *TEST_ADDR_16 = val_16; \
    *TEST_ADDR_8 = val_8; \
} while(0)

#define WRITE_C() do { \
    *TEST_ADDR_LCL = val_lcl; \
    *TEST_ADDR_8 = val_8; \
    *TEST_ADDR_32 = val_32; \
    *TEST_ADDR_16 = val_16; \
} while(0)

#define WRITE_D() do { \
    *TEST_ADDR_16 = val_16; \
    *TEST_ADDR_32 = val_32; \
    *TEST_ADDR_8 = val_8; \
    *TEST_ADDR_LCL = val_lcl; \
} while(0)

#define READ_A() do { \
    val_8 = *TEST_ADDR_8; \
    val_16 = *TEST_ADDR_16; \
    val_32 = *TEST_ADDR_32; \
    val_lcl = *TEST_ADDR_LCL; \
} while(0)

#define READ_B() do { \
    val_lcl = *TEST_ADDR_LCL; \
    val_32 = *TEST_ADDR_32; \
    val_16 = *TEST_ADDR_16; \
    val_8 = *TEST_ADDR_8; \
} while(0)

#define READ_C() do { \
    val_lcl = *TEST_ADDR_LCL; \
    val_8 = *TEST_ADDR_8; \
    val_32 = *TEST_ADDR_32; \
    val_16 = *TEST_ADDR_16; \
} while(0)

#define READ_D() do { \
    val_16 = *TEST_ADDR_16; \
    val_32 = *TEST_ADDR_32; \
    val_8 = *TEST_ADDR_8; \
    val_lcl = *TEST_ADDR_LCL; \
} while(0)

#define TEST(x, y) do { \
    WRITE_ ## x(); \
    READ_ ## y(); \
    COMPARE_ALL("R " ORDER_ ## x " W " ORDER_ ## y, i); \
} while(0)

/* Note no restore at end */
#define TEST_ALL(x) do { \
    TEST(x, A); \
    RESTORE_ALL(); \
    TEST(x, B); \
    RESTORE_ALL(); \
    TEST(x, C); \
    RESTORE_ALL(); \
    TEST(x, D); \
} while (0)

void ioTest(int iterations) {
    uint32_t val_bak[4];
    register uint8_t val_8 = 0x12;
    register uint16_t val_16 = 0x7dfe;
    register uint32_t val_32 = 0x0ce3fcbf;
    register uint32_t val_lcl = 0x4aa1f8ea;
    int i;

    /* Back up values to SRAM (stack) */
    val_bak[0] = val_8;
    val_bak[1] = val_16;
    val_bak[2] = val_32;
    val_bak[3] = val_lcl;

    for (i=0; i<iterations; i++) {
        /* The purpose of all these macros is to perform these accesses
         * as closely together and quickly as possible to attempt to
         * reproduce bus issues
         */

        TEST_ALL(A);
        TEST_ALL(B);
        TEST_ALL(C);
        TEST_ALL(D);

        INVERT_ALL();
        RESTORE_ALL();

        TEST_ALL(A);
        TEST_ALL(B);
        TEST_ALL(C);
        TEST_ALL(D);
    }
    printf("Done\n");
}