#include "task.h"
#include "os.h"
#include "armv7.h"

extern void task_entry_shim(void);

/*
stk 100
     -- Popped by task_entry_shim --
     FC pc_new
     F8 lr_new
     -- Popped by do_task_switch --
     F4 lr
     F0 r11
     EC r10
     E8 r9
     E4 r8
     E0 r7
     DC r6
     D8 r5
sp   D4 r4
*/

struct cpu_regs {
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;
    u32 r8;
    u32 r9;
    u32 r10;
    u32 r11;
    u32 lr;
    u32 lr_new;
    u32 pc_new;
};

struct task* current_task;
struct task* next_task;

void init_task(struct task *task, void (*func)(void), void* stack) {
    struct cpu_regs *regs;

    task->priority = 0;
    task->q = LIST_INIT(task->q);
    task->sp = (u32)stack - sizeof(struct cpu_regs);
    regs = (void*)task->sp;
    memset(regs, 0, sizeof(*regs));
    regs->lr = (u32)task_entry_shim;
    regs->pc_new = (u32)func;
    regs->lr_new = (u32)dequeue_current_task;
}

void task_switch(void) {
    struct task *old;
    /* TODO: assert interrupts enabled */
    /* TODO: assert current/next task runnable */
    /* disable interrupts. Interrups are reenabled in do_tasks_switch */
    disable_irq();
    if (next_task != current_task) {
        old = current_task;
        current_task = next_task;
        do_task_switch(current_task, old);
    }
}

