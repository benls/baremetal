#include "task.h"
#include "armv7.h"
#include "list.h"
#include "timer.h"
#include "io.h"

/* wait queue sorted by wakeup clock */
LIST(waitlist);

void waitqueue_add_release(struct list *q,
        struct cs_smp_lock *lock,
        u32 cpu_flags) {
    u32 cpu_flags_rq;
    /* remove current task from run queue */
    cpu_flags_rq = lock_runqueue();
    dequeue_current_task_locked();
    /* add to waitqueue */
    list_add(q, &current_task->q);
    cs_smp_rel(lock, cpu_flags_rq);
    /* reschedule and run next task */
    schedule_locked();
    rel_runqueue(cpu_flags);
    task_switch();
}

void waitqueue_del_release(struct list *q,
        struct cs_smp_lock *lock,
        u32 cpu_flags) {
    struct task *t;
    /* Remove task from waitqueue */
    list_del(q);
    cs_smp_rel(lock, cpu_flags);
    t = container_of(q, struct task, q);
    cpu_flags = lock_runqueue();
    /* add to runqueue */
    queue_task_locked(t);
    /* reschedule and run next task */
    schedule_locked();
    rel_runqueue(cpu_flags);
    task_switch();
}

void sleep_clks(u64 clks) {
    u32 cpu_flags;
    struct list *i;
    cpu_flags = lock_runqueue();
    /* add to sleepers list */
    /* The +1 is needed so we wait at least this many clocks */
    current_task->wake_clk = timer_get_clks() + clks + 1;
    LIST_FOREACH(i, &waitlist) {
        if (container_of(i, struct task, q)->wake_clk > current_task->wake_clk)
            break;
    }
    dequeue_current_task_locked();
    list_add(i->prev, &current_task->q);
    schedule_locked();
    rel_runqueue(cpu_flags);
    task_switch();
}

u64 wake_sleepers(void) {
    u64 clk;
    struct list *i;
    struct list *i_tmp;
    struct task *t;
    clk = timer_get_clks();
    LIST_FOREACH_SAFE(i, &waitlist, i_tmp) {
        t = container_of(i, struct task, q);
        /* remove from wait queue and add to run queue */
        if (clk >= t->wake_clk) {
            list_del(&t->q);
            queue_task_locked(t);
        } else {
            break;
        }
    }
    if (!list_empty(&waitlist)) {
        t = container_of(waitlist.next, struct task, q);
        return t->wake_clk - clk;
    }
    return (u64)~0;
}

