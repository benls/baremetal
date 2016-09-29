.globl do_task_switch
.globl task_entry_shim

.section ".text"

/*
 *  void do_task_switch(
 *          struct task *new,
 *          struct task *old );
*/
do_task_switch:
    /* Save callee saved registers and return address */
    push {r4-r11,lr}
    /* Save old task stack pointer. Allow old task to be null */
    cmp r1,#0
    strne sp,[r1]

    /* Load new task stack pointer */
    ldr sp,[r0]

    /* Reenable interrupts. Matches disable in task_switch */
    cpsie i 

    /* Load registers and return */
    pop {r4-r11,pc}

/* Entry point for new tasks.
 * Pop the real return address and pc from the stack.
*/
task_entry_shim:
    pop {lr,pc}


