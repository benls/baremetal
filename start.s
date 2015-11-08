
.globl main

/* Don't clear stack as part of bss */
.section ".mainstack"
.globl _main_stack
.balign 64
_main_stack: .skip 4096

.section ".text.boot"
.globl my_start
my_start:
    ldr r13, =_main_stack
    add r13, r13, #4096
    mov fp, #0
    mov r0, #0
    mov r1, #0
    mov r2, #0
    bl main
1:
    b 1b

