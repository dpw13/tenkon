include common.mak

NEWLIB := newlib/newlib

CONSOLE_LIBC := $(NEWLIB)/build/libc.a

NEWLIB_OPTS := \
	--host=m68k-linux-gnu \
	--enable-newlib-reent-small \
	--disable-newlib-fvwrite-in-streamio \
	--disable-newlib-fseek-optimization \
	--disable-newlib-wide-orient \
	--enable-newlib-nano-malloc \
	--disable-newlib-unbuf-stream-opt \
	--enable-lite-exit \
	--enable-newlib-global-atexit \
	--disable-newlib-io-float \
	--disable-newlib-supplied-syscalls \
	--enable-newlib-io-c99-formats

NEWLIB_CFLAGS := \
	-g -Os -m$(MACHINE) -msoft-float \
	-D__SINGLE_THREAD__ -DREENTRANT_SYSCALLS_PROVIDED -DNO_FORK \
	-UMISSING_SYSCALL_NAMES

all: hi.bin lo.bin fit/tenkon.itb

# Putting crt0.o first makes the objdump disasm clearer
src/console.elf: $(CONSOLE_LIBC)
	cd src && make

rom.bin: src/console.elf
	$(OBJCOPY) -O binary $< $@

hi.bin lo.bin: swizzle.py rom.bin $(UBOOT_SRC)/u-boot.bin
	python3 swizzle.py -o 0 -f rom.bin -o 0x8000 -f $(UBOOT_SRC)/u-boot.bin

$(NEWLIB)/build/Makefile:
	mkdir -p $(NEWLIB)/build
	cd $(NEWLIB)/build && CFLAGS="$(NEWLIB_CFLAGS)" ../configure $(NEWLIB_OPTS)

$(CONSOLE_LIBC): $(NEWLIB)/build/Makefile
	cd $(NEWLIB)/build && make -j8

fit/tenkon.itb:
	cd fit && make tenkon.itb

clean:
	cd src && make clean
	cd fit && make clean
	rm -f main *.o *.elf *.bin *.img *.map

distclean: clean
	rm -rf $(NEWLIB)/build
