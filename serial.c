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

void initializeSerial() {
    term_regs->A.CR = CR_CMD_RESET_TX;
    term_regs->A.CR = CR_CMD_RESET_RX;
    term_regs->A.CR = CR_CMD_RESET_MR0;
    term_regs->ACR = ACR_BAUDGEN_TABLE_0 | ACR_CTMODE_COUNTER_IP2;
    term_regs->A.CSR = CSR_BAUD_TX(0xB) | CSR_BAUD_RX(0xB);
    term_regs->A.MR = MR0_DIS_WDT | MR0_TX_FULL_THRESH_4 | MR0_BAUD_EXT2;
    term_regs->A.MR = MR1_EN_RX_RTS | MR1_NO_PARITY | MR1_BPC_8;
    term_regs->A.MR = MR2_DIS_TX_RTS | MR2_DIS_CTS_TX | MR2_STOP_LEN_1_00;
    term_regs->A.CR = CR_CMD_NONE | CR_ENA_RX | CR_ENA_TX;
}

void writeSerial(const char c) {
    // Busy wait until ready...probably not the best
    while( !(term_regs->A.SR & 0x04) ) {}
    term_regs->A.TxFIFO = c;
}

void writeStringToSerial(const char *buffer, const int length) {
    for(int i = 0; i < length; i++) {
        writeSerial(buffer[i]);
    }
}

void readSerial(char *c) {
    while( !(term_regs->A.SR & 0x01) ) {}
    *c = term_regs->A.RxFIFO;
    return;
}

int readFromSerial(char *buffer, const int size) {
    for(int i = 0; i < size; i++) {
        if( !(term_regs->A.SR & 0x01) ) {
            return i;
        }
        buffer[i] = term_regs->A.RxFIFO;
    }
    return size;
}

