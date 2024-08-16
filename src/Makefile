include ../common.mak

CONSOLE_LIBC_INC := -I$(NEWLIB)/libc/include -I$(NEWLIB)/build/targ-include
CONSOLE_LIBC := $(NEWLIB)/build/libc.a

# Putting crt0.o first makes the objdump disasm clearer
console.elf: crt0_mem.o exceptions.o console.o print_utils.o serial.o s1d13506_init.o test.o $(CONSOLE_LIBC) os_compat.o
	$(LD) -o $@ $^ $(LDFLAGS) -Wl,-Map=console.map

clean:
	rm -f main *.o *.elf *.map
