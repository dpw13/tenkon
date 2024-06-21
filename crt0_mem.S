/* Credit to https://github.com/tomstorey/m68k_bare_metal/blob/master/standalone/crt0.S */

        .title "crt0.S for m68k bare metal including memory initialization"

        /* A bunch of variables supplied by the linker */
        .extern _stack_end
        .extern _rom_base
        .extern _bss_start
        .extern _bss_end
        .extern _rodata_end
        .extern _data_start
        .extern _data_end
        .extern main

        .section .init
        .align 2

        .type _start, @function
        .globl _start
_start:
        move.w  #0x2700, %sr            /* Mask interrupts, supervisor mode */

        /* Initialise (clear) the BSS area */
1:      movea.l #_bss_start, %a0        /* Starting address */
        movea.l #_bss_end, %a1          /* End address */

2:      cmpa.l  %a1, %a0                /* Check if start < end */
        bge     7f

        clr.w   %a0@+                   /* Clear word and increment start */

        bra     2b

        /* Copy initialised data from ROM to RAM */

        /* Jump to main() */
7:      jmp     main

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