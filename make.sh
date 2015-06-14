#! /bin/bash

set -e
set -v

gcc_flags=" -g -ffreestanding -nostdlib -mcpu=cortex-a8 -march=armv7-a -marm -O0 "
c_flags=" -Wall -Wextra -std=gnu99 -pedantic -fno-strict-aliasing "

arm-none-eabi-gcc $gcc_flags -c start.s
arm-none-eabi-gcc $gcc_flags -c task_switch.s
arm-none-eabi-gcc $gcc_flags $c_flags -c main.c
arm-none-eabi-gcc $gcc_flags $c_flags -c uart.c
arm-none-eabi-gcc $gcc_flags $c_flags -c task.c
arm-none-eabi-gcc $gcc_flags -T start.ld -o start.elf start.o main.o uart.o task_switch.o task.o
arm-none-eabi-objcopy start.elf -O binary start.bin
python make_mlo.py
./block.sh
