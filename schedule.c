#include "os.h"
#include "task.h"
#include "armv7.h"
#include "timer.h"

static struct list runlists[N_PRIORITY];

void init_sched(void) {
    for(uint i = 0; i < cnt_of(runlists); i++)
        runlists[i] = LIST_INIT(runlists[i]);
    init_background();
}

void sched_start(void) {
    schedule();
    task_switch(); /* never returns */
}

void schedule(void) {
    u32 flags;
    u64 clk;
    u64 wake_clk;
    flags = disable_irq();
    /* current task goes to the back of its priority list */
    if (current_task && (current_task->flags & TASK_RUNNABLE)) {
        list_del(&current_task->q);
        list_add(runlists[current_task->priority].prev, &current_task->q);
    }

    /* wake up sleeping tasks */
    wake_clk = wake_sleepers();

    /* look for a new task to run */
    for (int i = 0; i < N_PRIORITY; i++) {
        if (!list_empty(&runlists[i])) {
            next_task = container_of(runlists[i].next, struct task, q);
            break;
        }
    }

    clk = us_to_clk(10 * 1000);
    if (wake_clk < clk)
        clk = wake_clk;
    /* schedule timer interrupt */
    timer_sched(clk); 
    set_cpsr(flags);
}

void queue_task_locked(struct task *task) {
    task->flags |= TASK_RUNNABLE;
    list_add(&runlists[task->priority], &task->q);
}

void queue_task(struct task *task) {
    u32 flags;
    flags = disable_irq();
    queue_task_locked(task);
    set_cpsr(flags);
}

