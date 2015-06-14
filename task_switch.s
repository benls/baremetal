@ void do_task_switch(struct task *new, struct task *old)
.globl do_task_switch
.section ".text"
do_task_switch:
    @ Save callee saved registers and return address
    stmdb sp!,{r4-r11,lr}
    @ Save old task stack pointer. Allow old task to be null
    cmp r1,#0
    strne sp,[r1]

    @ Load new task stack pointer
    ldr sp,[r0]
    @ Load registers and return
    ldmia sp!,{r4-r11,pc}

