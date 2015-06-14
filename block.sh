#! /bin/bash

set -v

dd if=/dev/zero bs=1M count=64 of=block
sudo losetup /dev/loop0 block
sudo fdisk /dev/loop0 <<EOF
n
p
1


a
t
b
w

EOF

sudo partprobe /dev/loop0
sudo mkfs.fat -F 32 /dev/loop0p1
mkdir -p mnt
sudo mount /dev/loop0p1 mnt
sudo cp MLO mnt
sudo umount mnt
sudo losetup -d /dev/loop0


