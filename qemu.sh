#! /bin/bash

~/qemu/bin/qemu-system-arm -M beaglexm -drive file=./block,if=sd,cache=writeback -clock unix --nographic -S -s
