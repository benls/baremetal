
.globl init_os

/* Don't clear stack as part of bss */
.section ".mainstack"
.balign 64
stack: .skip 4096

.section ".text.boot"
.globl _start
_start:
    ldr r0, =0xd3
    msr cpsr, r0
    ldr r13, =stack
    add r13, r13, #4096
    bl init_os

