/* Credit to https://github.com/tomstorey/m68k_bare_metal/blob/master/standalone/crt0.S */

        .title "Minimum crt0.S for m68k bare metal"

        .extern main
        .extern initializeSerial

        .section .init
        .align 2

        .type _start, @function
        .globl _start

        /* Jump to main() */
_start: jmp     main

        /* In a bare metal environment, main() should not return as there is
         * nowhere to resume execution from. Behaviour is undefined if this
         * happens! */

/*
 * __DefaultInterrupt handles all interrupt and exception vectors that have not
 * been overridden by the programmer.
 *
 * Unless handled more specifically, all exceptions and interrupts are masked
 * and the CPU stops executing.
 */
        .type __DefaultInterrupt, @function
        .weak __DefaultInterrupt
        .globl __DefaultInterrupt
__DefaultInterrupt:
        stop    #0x2700
        bra     .

        .end