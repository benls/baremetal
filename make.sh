#! /bin/bash

set -e
set -v

PREFIX=arm-none-eabi-
CC=${PREFIX}gcc
OBJCOPY=${PREFIX}objcopy
gcc_flags=" -g -mcpu=cortex-a8 -march=armv7-a -marm -O0 -nostartfiles "
c_flags=" -Wall -Wextra -std=gnu99 -pedantic -fno-strict-aliasing -I./include"

$CC $gcc_flags -c start.s
$CC $gcc_flags -c task_switch.s
$CC $gcc_flags $c_flags -c main.c
$CC $gcc_flags $c_flags -c uart.c
$CC $gcc_flags $c_flags -c task.c
$CC $gcc_flags $c_flags -c ./libc-helpers/file.c
$CC $gcc_flags $c_flags -c ./libc-helpers/sbrk.c
$CC $gcc_flags -T start.ld -o start.elf start.o main.o uart.o task_switch.o task.o file.o sbrk.o
$OBJCOPY start.elf -O binary start.bin
#python make_mlo.py
#./block.sh
