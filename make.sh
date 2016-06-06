#! /bin/bash

set -e
set -v

PREFIX=arm-none-eabi-
CC=${PREFIX}gcc
OBJCOPY=${PREFIX}objcopy
gcc_flags=" -g -mcpu=cortex-a8 -ffreestanding -fno-builtin -nostdlib -march=armv7-a -marm -O3"
c_flags=" -Wall -Wextra -std=gnu99 -pedantic -fno-strict-aliasing -I./include -I./tinyprintf -DTINYPRINTF_DEFINE_TFP_SPRINTF=0"
$CC $gcc_flags -c start.s
$CC $gcc_flags -c task_switch.s
$CC $gcc_flags -c vector-table.s
$CC $gcc_flags $c_flags -c main.c
$CC $gcc_flags $c_flags -c uart.c
$CC $gcc_flags $c_flags -c task.c
$CC $gcc_flags $c_flags -c timer.c
$CC $gcc_flags $c_flags -c ./intc_am335x.c
$CC $gcc_flags $c_flags -c ./oslib.c
$CC $gcc_flags $c_flags -c ./tinyprintf/tinyprintf.c
$CC $gcc_flags $c_flags -c ./schedule.c
$CC $gcc_flags $c_flags -c ./wait.c
$CC $gcc_flags $c_flags -c ./blink.c
$CC $gcc_flags $c_flags -c ./background.c
$CC $gcc_flags $c_flags -c ./print-ab.c
#TODO: find libgcc.a
$CC $gcc_flags -T start.ld -lgcc -Wl,-Map=output.map -o start.elf start.o main.o uart.o task_switch.o task.o timer.o vector-table.o intc_am335x.o oslib.o tinyprintf.o schedule.o wait.o blink.o background.o print-ab.o /usr/lib/gcc/arm-none-eabi/5.3.0/libgcc.a
$OBJCOPY start.elf -O binary start.bin

