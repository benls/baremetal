#include "task.h"
#include "armv7.h"
#include "list.h"
#include "timer.h"

/* wait queue sorted by wakeup clock */
LIST(waitlist);

void waitlist_add(u64 clks) {
    u32 flags;
    struct list *i;
    flags = disable_irq();
    /* TODO: assert task is runnable */
    /* remove current task from run queue */
    current_task->flags &= ~TASK_RUNNABLE;
    list_del(&current_task->q);
    /* add to sleepers list */
    /* The +1 is needed so we wait at least this many clocks */
    current_task->wake_clk = timer_get_clks() + clks + 1;
    LIST_FOREACH( i, &waitlist ) {
        if (container_of(i, struct task, q)->wake_clk > current_task->wake_clk)
            break;
    }
    list_add(i->prev, &current_task->q);
    set_cpsr(flags);
}

void sleep_clks(u64 clks) {
    waitlist_add(clks);
    schedule();
    task_switch();
}

u64 wake_sleepers(void) {
    u64 clk;
    struct list *i;
    struct task *t;
    clk = timer_get_clks();
    LIST_FOREACH( i, &waitlist ) {
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
        printf( "Clk: %llu; wake: %llu\r\n", clk, t->wake_clk );
        return t->wake_clk - clk;
    }
    printf( "Clk: %llu; No waiters\r\n", clk );
    return (u64)~0;
}

