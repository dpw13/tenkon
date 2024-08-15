#!/bin/busybox sh

# 1) Download a prebuilt BusyBox binary here:
#    https://busybox.net/downloads/binaries/
#
# 2) Prepare your kernel
#    cp /boot/vmlinuz vmlinuz
#
# 3) Copy the files
#    install -Dm0755 busybox-x86_64 initramfs/bin/busybox
#    install -Dm0755 init.sh initramfs/init
# 
# 4) Let's make it even smaller!
#    upx --brute initramfs/bin/busybox
#
# 5) Build the initramfs
#    cd initramfs
#    find . | sort | cpio -o -H newc -R 0:0 | gzip -9 > ../initramfs.img
#    cd ..
#
#    Yay! It is less than 512K!
#    -rw-r--r--  1 root  root    511K Jan  1 00:00 initramfs.img
#
# 6) Use QEMU (or KVM if available) to run your kernel
#    qemu-system-x86_64 -enable-kvm -kernel vmlinuz -initrd initramfs.img -serial stdio
#
# 7) Log in with root and no password

busybox rm /init
busybox mkdir -p /etc /proc /root /sbin /sys /usr/bin /usr/sbin
echo tty1::respawn:/sbin/getty 0 tty1 > /etc/inittab
echo tty2::respawn:/sbin/getty 0 tty2 >> /etc/inittab
echo tty3::respawn:/sbin/getty 0 tty3 >> /etc/inittab
echo tty4::respawn:/sbin/getty 0 tty4 >> /etc/inittab
echo tty5::respawn:/sbin/getty 0 tty5 >> /etc/inittab
echo tty6::respawn:/sbin/getty 0 tty6 >> /etc/inittab
echo ttyS0::respawn:/sbin/getty -L ttyS0 9600 vt100 >> /etc/inittab
echo Login with root and no password. > /etc/issue
echo >> /etc/issue
echo root::0:0:root:/root:/bin/sh > /etc/passwd
busybox mount -t proc proc /proc
busybox mount -t sysfs sys /sys
busybox mdev -s
busybox --install
hostname localhost
ip link set lo up
echo 5 > /proc/sys/kernel/printk
exec /linuxrc
