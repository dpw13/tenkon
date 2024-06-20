MACHINE=68030

CFLAGS=-Os -g -std=c99 -fomit-frame-pointer -m${MACHINE} -msoft-float
CXXFLAGS=-Os -g -fno-rtti -fno-exceptions -fomit-frame-pointer -m${MACHINE} -msoft-float
LDFLAGS=-g -fomit-frame-pointer -nostdlib -Wl,--build-id=none -m68030 -Wl,--script=m${MACHINE}.ld
PREFIX=m68k-linux-gnu

CC=$(PREFIX)-gcc
LD=$(PREFIX)-gcc
OBJCOPY=$(PREFIX)-objcopy
OBJDUMP=$(PREFIX)-objdump

all: hi.bin lo.bin

# Putting crt0.o first makes the objdump disasm clearer
main.elf: crt0.o main.o
	$(LD) -o $@ $^ $(LDFLAGS)

rom.bin: main.elf
	$(OBJCOPY) -O binary $< $@

hi.bin lo.bin: swizzle.py rom.bin
	python3 swizzle.py rom.bin

clean:
	rm -f main *.o *.elf *.bin