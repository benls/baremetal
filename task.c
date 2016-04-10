#include "task.h"
#include "os.h"
#include "armv7.h"

/*
stk 100
     FC r14
     F8 r11
     F4 r10
     F0 r9
     EC r8
     E8 r7
     E4 r6
     E0 r5
sp   DC r4
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
    regs->lr = (u32)func;
}

void task_switch(void) {
    struct task *old;
    if (next_task != current_task) {
        old = current_task;
        current_task = next_task;
        do_task_switch(current_task, old);
    }
}

