//
// Authors: dpw13, kitsune
// Created by kitsune on 6/22/2024.
//

#include "serial.h"
#include "sc26c92.h"

#define duart_regs ((volatile duart_regs_t *)0xFE000000)
#define term_regs  ((volatile duart_regs_t *)0xFE400000)
#define rtc_regs ((void *)0xFE800000)
#define ps2_regs ((void *)0xFEC00000)

/* Change these defines to change the active console */
#define console_regs (term_regs)
#define console_port (console_regs->B)

void initializeSerial() {
    /* The baud rate selection is specific to channel A */
    console_regs->A.CR = CR_CMD_RESET_MR0;
    console_regs->A.MR = MR0_DIS_WDT | MR0_TX_FULL_THRESH_4 | MR0_BAUD_EXT2;
    console_regs->ACR = ACR_BAUDGEN_TABLE_0 | ACR_CTMODE_COUNTER_IP2;

    console_port.CR = CR_CMD_RESET_TX;
    console_port.CR = CR_CMD_RESET_RX;
    console_port.CR = CR_CMD_RESET_MR0;
    console_port.CSR = CSR_BAUD_TX(CSR_BAUD_EXT2_115_2K) | CSR_BAUD_RX(CSR_BAUD_EXT2_115_2K);
    console_port.MR = MR0_DIS_WDT | MR0_TX_FULL_THRESH_4 | MR0_BAUD_EXT2;
    console_port.MR = MR1_EN_RX_RTS | MR1_NO_PARITY | MR1_BPC_8;
    console_port.MR = MR2_DIS_TX_RTS | MR2_DIS_CTS_TX | MR2_STOP_LEN_1_00;
    console_port.CR = CR_CMD_NONE | CR_ENA_RX | CR_ENA_TX;
}

void forceWriteSerial(const char c) {
    console_port.TxFIFO = c;
}

static inline void _writeSerial(const char c) {
    // Busy wait until ready...probably not the best
    while( !(console_port.SR & SR_TXRDY) ) {}
    console_port.TxFIFO = c;
}

void writeSerial(const char c) {
    if (c == '\n')
        _writeSerial('\r');
    _writeSerial(c);
}

void writeStringToSerial(const char *buffer, const int length) {
    const char *p = buffer;
    for(int i = 0; i < length; i++) {
        if (*p == '\n')
            _writeSerial('\r');
        _writeSerial(*p++);
    }
}

unsigned char serialGetError(void) {
    uint8_t ret = console_port.SR & SR_ERR_MASK;
    // Clear errors
    console_port.CR = CR_CMD_RESET_ERR;
    return ret;
}

char readSerial(void) {
    while( !(console_port.SR & SR_RXRDY) ) {}
    return console_port.RxFIFO;
}

int readFromSerial(char *buffer, const int size) {
    for(int i = 0; i < size; i++) {
        if( !(console_port.SR & SR_RXRDY) ) {
            return i;
        }
        buffer[i] = console_port.RxFIFO;
    }
    return size;
}

