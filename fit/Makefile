include ../common.mak

MKIMAGE=$(UBOOT_SRC)/tools/mkimage

TENKON_DTB=$(LINUX_SRC)/arch/m68k/boot/dts/tenkon/tenkon.dtb

initramfs.cpio.gz:
	rm -rf initramfs
	mkdir -p initramfs
	install -Dm0755 $(BUSYBOX_SRC)/busybox initramfs/bin/busybox
	install -Dm0755 init.sh initramfs/init
	cd initramfs && find . | sort | cpio -o -H newc -R 0:0 | gzip -9 > ../$@

# I really wish there was a way to specify search paths with mkimage, for the
# included files or for DTC
tenkon.itb: fit_kernel_dtb.its initramfs.cpio.gz $(LINUX_SRC)/vmlinux.gz $(TENKON_DTB)
	cp $(LINUX_SRC)/vmlinux.gz .
	cp $(TENKON_DTB) .
	PATH=PATH:$(UBOOT_SRC)/scripts/dtc $(MKIMAGE) -f fit_kernel_dtb.its $@

clean:
	rm -rf initramfs
	rm -f *.gz *.itb
