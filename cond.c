#include "task.h"
#include "armv7.h"

void cond_wait(struct cond* cond) {
    //TODO: assert cond task == NULL
    u32 flags;
    flags = disable_irq();
    dequeue_current_task_locked();
    cond->task = current_task;
    set_cpsr(flags);
    schedule();
    task_switch();
}

void cond_signal(struct cond* cond) {
    u32 flags;
    flags = disable_irq();
    if (cond->task) {
        queue_task_locked(cond->task);
        cond->task = NULL;
    }
    set_cpsr(flags);
}

