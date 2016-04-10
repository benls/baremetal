#include "os.h"
#include "task.h"
#include "armv7.h"
#include "timer.h"

static u64 background_stack[50];
static void background_func(void) {
    for(;;);
}
static struct task background_task;

static struct list runlists[N_PRIORITY];

void init_sched(void) {
    for(uint i = 0; i < cnt_of(runlists); i++)
        runlists[i] = LIST_INIT(runlists[i]);
    init_task(&background_task,
            background_func,
            &background_stack[cnt_of(background_stack) - 2]);
    background_task.priority = N_PRIORITY - 1;
    queue_task(&background_task);
}

void sched_start(void) {
    schedule();
    task_switch();
}

void schedule(void) {
    u32 flags;
    flags = disable_irq();
    /* current task goes to the back of its priority list */
    if (current_task) {
        list_del(&current_task->q);
        list_add(runlists[current_task->priority].prev, &current_task->q);
    }
    /* look for a new task to run */
    for (int i = 0; i < N_PRIORITY; i++) {
        if (runlists[i].next != &runlists[i]) {
            next_task = container_of(runlists[i].next, struct task, q);
            break;
        }
    }
    /* schedule this function to be called again later */
    timer_sched(us_to_clk(1000 * 1000)); /*TODO: decrease */
    set_cpsr(flags);
}

void queue_task(struct task *task) {
    u32 flags;
    flags = disable_irq();
    list_add(&runlists[task->priority], &task->q);
    set_cpsr(flags);
}

