
.globl main

.section ".bss"
.globl _main_stack
.balign 64
_main_stack: .skip 4096

.section ".text.boot"
.globl _start
_start:
    ldr r13,    =_main_stack
    bl  main
1:
    b 1b


