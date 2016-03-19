#! /bin/bash

set -e
set -v

PREFIX=arm-none-eabi-
CC=${PREFIX}gcc
OBJCOPY=${PREFIX}objcopy
gcc_flags=" -g -mcpu=cortex-a8 -march=armv7-a -marm -O0"
c_flags=" -Wall -Wextra -std=gnu99 -pedantic -fno-strict-aliasing -I./include"

$CC $gcc_flags -c start.s
$CC $gcc_flags -c task_switch.s
$CC $gcc_flags -c vector-table.s
$CC $gcc_flags $c_flags -c main.c
$CC $gcc_flags $c_flags -c uart.c
$CC $gcc_flags $c_flags -c task.c
$CC $gcc_flags $c_flags -c timer.c
$CC $gcc_flags $c_flags -c ./libc-helpers/file.c
$CC $gcc_flags $c_flags -c ./libc-helpers/sbrk.c
$CC $gcc_flags $c_flags -c ./debug/debug.c
$CC $gcc_flags $c_flags -c ./interrupt.c
$CC $gcc_flags $c_flags -c ./intc_am335x.c
$CC $gcc_flags -T start.ld -Wl,-Map=output.map -o start.elf start.o main.o uart.o task_switch.o task.o file.o sbrk.o debug.o timer.o vector-table.o interrupt.o intc_am335x.o
$OBJCOPY start.elf -O binary start.bin
#python make_mlo.py
#./block.sh
