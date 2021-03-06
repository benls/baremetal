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

void schedule_locked(void) {
    u64 clk;
    u64 wake_clk;
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
}

void schedule(void) {
    u32 cpu_flags;
    cpu_flags = lock_runqueue();
    schedule_locked();
    rel_runqueue(cpu_flags);
}

void queue_task_locked(struct task *task) {
    task->flags |= TASK_RUNNABLE;
    list_add(&runlists[task->priority], &task->q);
}

void dequeue_current_task_locked(void) {
    current_task->flags &= ~TASK_RUNNABLE;
    list_del(&current_task->q);
}

void queue_task(struct task *task) {
    u32 flags;
    flags = lock_runqueue();
    queue_task_locked(task);
    rel_runqueue(flags);
}

void dequeue_current_task(void) {
    u32 flags;
    flags = lock_runqueue();
    dequeue_current_task_locked();
    schedule_locked();
    rel_runqueue(flags);
    task_switch();
}
