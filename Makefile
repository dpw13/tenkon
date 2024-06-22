MACHINE=68030

LIBC_INC=-Inewlib/newlib/libc/include -Inewlib/newlib/build/targ-include
LIBC=newlib/newlib/build/libc.a

CFLAGS=-Os -g -std=c99 -fdata-sections -ffunction-sections -fomit-frame-pointer -m${MACHINE} -msoft-float $(LIBC_INC)
CXXFLAGS=$(CFLAGS) -nostdinc++ -fno-rtti -fno-exceptions
LDFLAGS=-g -Os -fomit-frame-pointer -nostdlib -Wl,--gc-sections -Wl,--build-id=none -m68030 -Wl,--script=build/m${MACHINE}.ld
PREFIX=m68k-linux-gnu

CC=$(PREFIX)-gcc
LD=$(PREFIX)-gcc
OBJCOPY=$(PREFIX)-objcopy
OBJDUMP=$(PREFIX)-objdump

NEWLIB_OPTS= \
	--host=m68k-linux-gnu \
	--enable-newlib-reent-small \
	--disable-newlib-fvwrite-in-streamio \
	--disable-newlib-fseek-optimization \
	--disable-newlib-wide-orient \
	--enable-newlib-nano-malloc \
	--disable-newlib-unbuf-stream-opt \
	--enable-lite-exit \
	--enable-newlib-global-atexit \
	--enable-newlib-nano-formatted-io \
	--disable-newlib-io-float \
	--disable-newlib-supplied-syscalls

NEWLIB_CFLAGS= \
	-g -Os -m$(MACHINE) -msoft-float \
	-D__SINGLE_THREAD__ -DREENTRANT_SYSCALLS_PROVIDED -DNO_FORK \
	-UMISSING_SYSCALL_NAMES

all: hi.bin lo.bin

# Putting crt0.o first makes the objdump disasm clearer
main.elf: crt0.o main.o serial.o $(LIBC) os_compat.o
	$(LD) -o $@ $^ $(LDFLAGS)

rom.bin: main.elf
	$(OBJCOPY) -O binary $< $@

hi.bin lo.bin: swizzle.py rom.bin
	python3 swizzle.py rom.bin

newlib/newlib/build/Makefile:
	mkdir -p newlib/newlib/build
	cd newlib/newlib/build && CFLAGS="$(NEWLIB_CFLAGS)" ../configure $(NEWLIB_OPTS)

$(LIBC): newlib/newlib/build/Makefile
	cd newlib/newlib/build && make -j8

clean:
	rm -f main *.o *.elf *.bin
	rm -rf newlib/newlib/build