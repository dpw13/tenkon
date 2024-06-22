#include <stdio.h>
#include <stdint.h>

#if 1

typedef struct {
    uint8_t MR;
    union {
        uint8_t SR; // ro
        uint8_t CSR; // wo
    };
    uint8_t CR; // wo
    union {
        uint8_t RxFIFO; // ro
        uint8_t TxFIFO; // wo
    };
} uart_regs_t;

typedef struct {
    uart_regs_t A; // UART A regs
    union {
        uint8_t IPCR; // ro
        uint8_t ACR; // wo
    };
    union {
        uint8_t ISR; // ro
        uint8_t IMR; // wo
    };
    union {
        uint8_t CTU; // ro
        uint8_t CTPU; // wo
    };
    union {
        uint8_t CTL; // ro
        uint8_t CTPL; // wo
    };
    uart_regs_t B; // UART B regs
    uint8_t USER;
    union {
        uint8_t IPR; // ro
        uint8_t OPCR; // wo
    };
    union {
        uint8_t START; // ro
        uint8_t SOP12; // wo
    };
    union {
        uint8_t STOP; // ro
        uint8_t ROP12; // wo
    };
} duart_regs_t;



// Controlled by IO board U10 (8BIT_CS.PLD)
#define duart_regs ((volatile duart_regs_t *)0xFE000000)
#define term_regs  ((volatile duart_regs_t *)0xFE400000)
#define rtc_regs ((void *)0xFE800000)
#define ps2_regs ((void *)0xFEC00000)

#define CR_CMD_NONE      (0x0 << 4)
#define CR_CMD_RESET_MR1 (0x1 << 4) // "set pointer" sets to MR1
#define CR_CMD_RESET_RX  (0x2 << 4)
#define CR_CMD_RESET_TX  (0x3 << 4)
#define CR_CMD_RESET_ERR (0x3 << 4)
#define CR_CMD_RESET_MR0 (0xB << 4)

#define CR_ENA_RX (1 << 0)
#define CR_DIS_RX (1 << 1)
#define CR_ENA_TX (1 << 2)
#define CR_DIS_TX (1 << 3)

#define ACR_BAUDGEN_TABLE_0 (0 << 7)
#define ACR_BAUDGEN_TABLE_1 (1 << 7)

#define ACR_CTMODE_COUNTER_IP2      (0 << 4)
#define ACR_CTMODE_COUNTER_TXCA     (1 << 4)
#define ACR_CTMODE_COUNTER_TXCB     (2 << 4)
#define ACR_CTMODE_COUNTER_CLK      (3 << 4)
#define ACR_CTMODE_TIMER_IP2_DIV_1  (4 << 4)
#define ACR_CTMODE_TIMER_IP2_DIV_16 (5 << 4)
#define ACR_CTMODE_TIMER_CLK_DIV_1  (6 << 4)
#define ACR_CTMODE_TIMER_CLK_DIV_16 (7 << 4)

#define CSR_BAUD_EXT2_115_2K    0x6
#define CSR_BAUD_EXT2_57_6K     0x8 4f7a4f7a 01001111 01111010
#define CSR_BAUD_EXT2_38_4K     0xC
#define CSR_BAUD_EXT2_28_8K     0x4
#define CSR_BAUD_EXT2_9600      0xB
#define CSR_BAUD_TX(x)  ((x) << 0)
#define CSR_BAUD_RX(x)  ((x) << 4)

#define MR0_EN_WDT  (1 << 7)
#define MR0_DIS_WDT (0 << 7)
#define MR0_TX_FULL_THRESH_8 (0 << 4)
#define MR0_TX_FULL_THRESH_4 (1 << 4)
#define MR0_TX_FULL_THRESH_6 (2 << 4)
#define MR0_TX_FULL_THRESH_1 (3 << 4)
#define MR0_BAUD_NORMAL (0)
#define MR0_BAUD_EXT    (1)
#define MR0_BAUD_EXT2   (4)

#define MR1_DIS_RX_RTS  (0 << 7)
#define MR1_EN_RX_RTS   (1 << 7)
#define MR1_PARITY_EVEN (0 << 2)
#define MR1_PARITY_ODD  (1 << 2)
#define MR1_WITH_PARITY  (0 << 3)
#define MR1_FORCE_PARITY (1 << 3)
#define MR1_NO_PARITY    (2 << 3)
#define MR1_BPC_5   0
#define MR1_BPC_6   1
#define MR1_BPC_7   2
#define MR1_BPC_8   3

#define MR2_DIS_TX_RTS  (0 << 5)
#define MR2_EN_TX_RTS   (1 << 5)
#define MR2_DIS_CTS_TX  (0 << 4)
#define MR2_EN_CTS_TX   (1 << 4)

#define MR2_STOP_LEN_1_00   (7)

void main(void) {
    term_regs->A.CR = CR_CMD_RESET_TX;
    term_regs->A.CR = CR_CMD_RESET_RX;
    term_regs->A.CR = CR_CMD_RESET_MR0;
    term_regs->ACR = ACR_BAUDGEN_TABLE_0 | ACR_CTMODE_COUNTER_IP2;
    term_regs->A.CSR = CSR_BAUD_TX(0xB) | CSR_BAUD_RX(0xB);
    term_regs->A.MR = MR0_DIS_WDT | MR0_TX_FULL_THRESH_4 | MR0_BAUD_EXT2;
    term_regs->A.MR = MR1_EN_RX_RTS | MR1_NO_PARITY | MR1_BPC_8;
    term_regs->A.MR = MR2_DIS_TX_RTS | MR2_DIS_CTS_TX | MR2_STOP_LEN_1_00;
    term_regs->A.CR = CR_CMD_NONE | CR_ENA_RX | CR_ENA_TX;

    while (1) {
        //register uint8_t unused = term_regs->A.SR;
        if (term_regs->A.SR & 0x04)
            term_regs->A.TxFIFO = 0x5A;
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