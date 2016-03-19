#ifndef TASK_H
#define TASK_H

#include "os.h"

struct task {
    u32 sp;
    struct task *next;
};

extern struct task *current_task;

struct task* new_task(struct task *task, void (*func)(void), void* stack);
void queue_task(struct task* task);
void task_switch(void);
void do_task_switch(struct task *new, struct task *old);

#endif
