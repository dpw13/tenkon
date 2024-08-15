GIT_ROOT := $(shell git rev-parse --show-toplevel)

MACHINE=68030

OPT=-Os
PLAT_CFLAGS=-finline-limit=1000 -fdata-sections -ffunction-sections
# Omit -msep-data as it produces a global offset table that takes up additional SRAM
MACH_CFLAGS=-m${MACHINE} -msoft-float

CFLAGS=-g -Wall -Wno-char-subscripts $(OPT) $(PLAT_CFLAGS) $(MACH_CFLAGS) -std=gnu99 $(CONSOLE_LIBC_INC)
CXXFLAGS=$(CFLAGS) -nostdinc++ -fno-rtti -fno-exceptions
LDFLAGS=-g $(OPT) -fomit-frame-pointer -nostdlib -Wl,--gc-sections -Wl,--build-id=none -Wl,--discard-locals -m68030 -Wl,--script=$(GIT_ROOT)/build/m${MACHINE}.ld
CROSS_COMPILE=m68k-linux-gnu

CC=$(CROSS_COMPILE)-gcc
LD=$(CROSS_COMPILE)-gcc
OBJCOPY=$(CROSS_COMPILE)-objcopy
OBJDUMP=$(CROSS_COMPILE)-objdump

NEWLIB := $(GIT_ROOT)/newlib/newlib
UBOOT_SRC := $(GIT_ROOT)/../u-boot
LINUX_SRC := $(GIT_ROOT)/../linux
BUSYBOX_SRC := $(GIT_ROOT)/../busybox