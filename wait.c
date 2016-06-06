#include "task.h"
#include "armv7.h"
#include "list.h"
#include "timer.h"

/* wait queue sorted by wakeup clock */
LIST(waitlist);

//TODO: spinlock release
void waitqueue_add(struct list *q, u32 flags) {
    /* TODO: assert task is runnable */
    /* remove current task from run queue */
    dequeue_current_task_locked();
    list_add(q, &current_task->q);
    set_cpsr(flags);
    schedule();
    task_switch();
}

void sleep_clks(u64 clks) {
    u32 flags;
    struct list *i;
    flags = disable_irq();
    /* add to sleepers list */
    /* The +1 is needed so we wait at least this many clocks */
    current_task->wake_clk = timer_get_clks() + clks + 1;
    LIST_FOREACH( i, &waitlist ) {
        if (container_of(i, struct task, q)->wake_clk > current_task->wake_clk)
            break;
    }
    waitqueue_add(i->prev, flags);
}

u64 wake_sleepers(void) {
    u64 clk;
    struct list *i;
    struct list *i_tmp;
    struct task *t;
    clk = timer_get_clks();
    LIST_FOREACH_SAFE( i, &waitlist, i_tmp ) {
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

