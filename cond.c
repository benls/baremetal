#include "task.h"
#include "armv7.h"
#include "printf.h"

void cond_wait(struct cond* cond, struct cs_smp_lock *lock, u32 cpu_flags) {
    u32 cpu_flags_rq;
    cpu_flags_rq = lock_runqueue();
    dequeue_current_task_locked();
    cond->task = current_task;
    cs_smp_rel(lock, cpu_flags_rq);
    schedule_locked();
    rel_runqueue(cpu_flags);
    task_switch();
}

void cond_signal(struct cond* cond) {
    u32 cpu_flags;
    if (cond->task) {
        cpu_flags = lock_runqueue();
        queue_task_locked(cond->task);
        schedule_locked();
        rel_runqueue(cpu_flags);
        cond->task = NULL;
    }
}

