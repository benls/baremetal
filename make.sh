#! /bin/bash

set -e
set -v

PREFIX=/opt/arm-none-eabi-5.2.0/bin/arm-none-eabi-
CC=${PREFIX}gcc
OBJCOPY=${PREFIX}objcopy
gcc_flags=" -g -ffreestanding -nostdlib -mcpu=cortex-a8 -march=armv7-a -marm -O0 "
c_flags=" -Wall -Wextra -std=gnu99 -pedantic -fno-strict-aliasing "

$CC $gcc_flags -c start.s
$CC $gcc_flags -c task_switch.s
$CC $gcc_flags $c_flags -c main.c
$CC $gcc_flags $c_flags -c uart.c
$CC $gcc_flags $c_flags -c task.c
$CC $gcc_flags -T start.ld -o start.elf start.o main.o uart.o task_switch.o task.o
$OBJCOPY start.elf -O binary start.bin
#python make_mlo.py
#./block.sh
