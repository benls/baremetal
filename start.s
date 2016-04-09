
.globl main

/* Don't clear stack as part of bss */
.section ".mainstack"
.balign 64
_main_stack: .skip 4096

.section ".text.boot"
.globl _start
_start:
    ldr r0, =0xdf
    msr cpsr, r0
    ldr r13, =_main_stack
    add r13, r13, #4096 /*TODO*/
    bl main
1:
    b 1b

