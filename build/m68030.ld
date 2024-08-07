/* Linker script for bare-metal ROM generation */

_rom_sz = 0x100000;
_sram_sz = 0x080000;
_stack_sz = 4K;

MEMORY {
    rom     (rx!w) : ORIGIN = 0xFFF00000, LENGTH = _rom_sz
    sram    (rwx)  : ORIGIN = 0x00000000, LENGTH = _sram_sz
}

/* Section Definitions */
/* Credit to https://github.com/tomstorey/m68k_bare_metal/blob/master/standalone/platform.ld */
SECTIONS {
    .evt_rom : {
        _evt_start = .;

        /* Reset vector */
        LONG(_stack_end);                /* Initial SSP */
        LONG(ABSOLUTE(_start));         /* Initial PC */

        /* Exception vector table */
        LONG(DEFINED(BusError) ? ABSOLUTE(BusError) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(AddressError) ? ABSOLUTE(AddressError) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(IllegalInstruction) ? ABSOLUTE(IllegalInstruction) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(ZeroDivide) ? ABSOLUTE(ZeroDivide) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(CHKInstruction) ? ABSOLUTE(CHKInstruction) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAPVInstruction) ? ABSOLUTE(TRAPVInstruction) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(PrivilegeViolation) ? ABSOLUTE(PrivilegeViolation) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(Trace) ? ABSOLUTE(Trace) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(Line1010Emulator) ? ABSOLUTE(Line1010Emulator) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(Line1111Emulator) ? ABSOLUTE(Line1111Emulator) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(HardwareBreakpoint) ? ABSOLUTE(HardwareBreakpoint) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(CoprocessorProtocolViolation) ? ABSOLUTE(CoprocessorProtocolViolation) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(FormatError) ? ABSOLUTE(FormatError) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(UninitializedInterruptVector) ? ABSOLUTE(UninitializedInterruptVector) : ABSOLUTE(__DefaultInterrupt));
        LONG(0); /* Unassigned, reserved */
        LONG(0); /* Unassigned, reserved */
        LONG(0); /* Unassigned, reserved */
        LONG(0); /* Unassigned, reserved */
        LONG(0); /* Unassigned, reserved */
        LONG(0); /* Unassigned, reserved */
        LONG(0); /* Unassigned, reserved */
        LONG(0); /* Unassigned, reserved */
        LONG(DEFINED(SpuriousInterrupt) ? ABSOLUTE(SpuriousInterrupt) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(IRQ1) ? ABSOLUTE(IRQ1) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(IRQ2) ? ABSOLUTE(IRQ2) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(IRQ3) ? ABSOLUTE(IRQ3) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(IRQ4) ? ABSOLUTE(IRQ4) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(IRQ5) ? ABSOLUTE(IRQ5) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(IRQ6) ? ABSOLUTE(IRQ6) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(IRQ7) ? ABSOLUTE(IRQ7) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP0) ? ABSOLUTE(TRAP0) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP1) ? ABSOLUTE(TRAP1) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP2) ? ABSOLUTE(TRAP2) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP3) ? ABSOLUTE(TRAP3) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP4) ? ABSOLUTE(TRAP4) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP5) ? ABSOLUTE(TRAP5) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP6) ? ABSOLUTE(TRAP6) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP7) ? ABSOLUTE(TRAP7) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP8) ? ABSOLUTE(TRAP8) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP9) ? ABSOLUTE(TRAP9) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP10) ? ABSOLUTE(TRAP10) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP11) ? ABSOLUTE(TRAP11) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP12) ? ABSOLUTE(TRAP12) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP13) ? ABSOLUTE(TRAP13) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP14) ? ABSOLUTE(TRAP14) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(TRAP15) ? ABSOLUTE(TRAP15) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(FPBranchOrSetUnorderedCondition) ? ABSOLUTE(FPBranchOrSetUnorderedCondition) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(FPInexactResult) ? ABSOLUTE(FPInexactResult) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(FPZeroDivide) ? ABSOLUTE(FPZeroDivide) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(FPUnderflow) ? ABSOLUTE(FPUnderflow) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(FPOperandError) ? ABSOLUTE(FPOperandError) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(FPOverflow) ? ABSOLUTE(FPOverflow) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(FPSignallingNAN) ? ABSOLUTE(FPSignallingNAN) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(FPUnimplementedDataType) ? ABSOLUTE(FPUnimplementedDataType) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(MMUConfigurationError) ? ABSOLUTE(MMUConfigurationError) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(MMUIllegalOperation) ? ABSOLUTE(MMUIllegalOperation) : ABSOLUTE(__DefaultInterrupt));
        LONG(DEFINED(MMUAccessLevelViolation) ? ABSOLUTE(MMUAccessLevelViolation) : ABSOLUTE(__DefaultInterrupt));

        . = 0x100;
/*
 *
 * User Interrupt Vectors
 *
 *   Vector #         Address
 *   Hex    Dec       Hex      Dec
 *   40-FF  64-255    100-3FC  256-1020
 *
 * To calculate the offset within the .evt_rom section to place your vector, use
 * the following calculation (assuming vector number = 0x50):
 *
 *   0x50 * 4 = 0x140
 *
 * You could then place your vector at that position in the vector table as
 * follows:
 *
 *   . = 0x140;
 *   LONG(ABSOLUTE(NameOfHandlerFunction));
 */
        . = 0x300;
        /* Set default interrupt for all 8-bit board interrupts */
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));
        LONG(ABSOLUTE(__DefaultInterrupt));

        . = 0x3fc;
        LONG(ABSOLUTE(__DefaultInterrupt));

        . = 0x400;
        _evt_end = .;
    } > rom

    .text : {
        _text_start = .;
        KEEP(*(.init))
        *(.text .text.*)
        . = ALIGN(0x10);
        _text_end = .;
    } > rom

    .rodata : AT(_text_end) {
        _rodata_start = .;
        *(.rodata .rodata.*)
        . = ALIGN(0x10);
        _rodata_end = .;
    } > rom

    .evt_ram : {
        /* Reserve space for exception vectors in RAM */
        . = 0x400;
    } > sram

    .data : AT(_rodata_end) {
        _data_start = .;
        *(.data .data.*)
        . = ALIGN(0x10);
        _data_end = .;
    } > sram

    .bss : {
        _bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(0x10);
        _bss_end = .;
    } > sram

    .heap : {
        _heap_start = .;
        . += _sram_sz - _stack_sz - SIZEOF(.bss) - SIZEOF(.data) - SIZEOF(.evt_ram);
        _heap_end = .;
    } > sram

    .stack : {
        _stack_start = .;
        . += _stack_sz;
        _stack_end = .;
    } > sram

}