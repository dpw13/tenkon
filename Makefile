MACHINE=68030

CFLAGS=-s -Os -std=c99 -fomit-frame-pointer -m${MACHINE} -msoft-float
CXXFLAGS=-Os -fno-rtti -fno-exceptions -fomit-frame-pointer -m${MACHINE} -msoft-float
LDFLAGS=-s -Os -nostdlib -Wl,--build-id=none -fomit-frame-pointer -m68000 -Wl,--traditional-format -Wl,--script=m${MACHINE}.ld
PREFIX=m68k-linux-gnu

CC=$(PREFIX)-gcc
LD=$(PREFIX)-ld
OBJCOPY=$(PREFIX)-objcopy
OBJDUMP=$(PREFIX)-objdump

all: rom.bin

rom.bin: main
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f main *.o *.bin