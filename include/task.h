#ifndef TASK_H
#define TASK_H

#include "os.h"
#include "list.h"

/* last priority is used by background task */
#define N_PRIORITY 2

struct task {
    u32 sp;
    struct list q;
    uint priority;
};

extern struct task *current_task;
extern struct task *next_task;

void init_task(struct task *task, void (*func)(void), void* stack);
void init_sched(void);
void queue_task(struct task* task);
void task_switch(void);
void do_task_switch(struct task *new, struct task *old);
void sched_start(void);
void schedule(void);

#endif
