/* Credit to https://github.com/tomstorey/m68k_bare_metal/blob/master/standalone/crt0.S */

        .title "crt0.S for m68k bare metal including memory initialization"

        /* A bunch of variables supplied by the linker */
        .extern _evt_start
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

        /* Copy event table to RAM */
Levt_table:
        movea.l #_evt_start, %a0       /* Source address */
        movea.l #0, %a1                 /* Destination start address */
        movea.l #400, %a2               /* Destination end address */
Levt_loop:
        cmpa.l  %a2, %a1                /* Check if start < end */
        bge     Lbss_init

        move.w  %a0@+, %a1@+            /* Copy a word from ROM to RAM */

        bra     Levt_loop

        /* Initialise (clear) the BSS area */
Lbss_init:
        movea.l #_bss_start, %a0        /* Starting address */
        movea.l #_bss_end, %a1          /* End address */

Lbss_loop:      cmpa.l  %a1, %a0                /* Check if start < end */
        bge     Ldata_init

        clr.w   %a0@+                   /* Clear word and increment start */

        bra     Lbss_loop

        /* Copy initialised data from ROM to RAM */
Ldata_init:
        /* This is not a typo. Initialized data is stored after _rodata_end
         * in the ROM. */
        movea.l #_rodata_end, %a0       /* Source address */
        movea.l #_data_start, %a1       /* Destination start address */
        movea.l #_data_end, %a2         /* Destination end address */

Ldata_loop:
        cmpa.l  %a2, %a1                /* Check if start < end */
        bge     Lmain

        move.w  %a0@+, %a1@+            /* Copy a word from ROM to RAM */

        bra     Ldata_loop

        /* Jump to main() */
Lmain:  jmp     main

        /* In a bare metal environment, main() should not return as there is
         * nowhere to resume execution from. Behaviour is undefined if this
         * happens! */

        .end