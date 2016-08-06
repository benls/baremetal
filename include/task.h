#ifndef TASK_H
#define TASK_H

#include "os.h"
#include "list.h"
#include "armv7.h"

/* last priority is used by background task */
#define N_PRIORITY 2

struct task {
    u32 sp;
    struct list q;
    u64 wake_clk;
    uint priority;
    u32 flags;
#define TASK_RUNNABLE 1
};

#define SEM_NONBLOCK 1

struct sem {
    struct cs_smp_lock lock;
    uint count;
    struct list list;
};

struct cond {
    struct task * task;
};

extern struct task *current_task;
extern struct task *next_task;
extern struct cs_smp_lock runqueue_lock;

static inline u32 lock_runqueue(void) {
    return cs_smp_aq(&runqueue_lock);
}
static inline void rel_runqueue(u32 cpu_flags) {
    cs_smp_rel(&runqueue_lock, cpu_flags);
}

void init_task(struct task *task, void (*func)(void), void* stack);
void init_sched(void);
void queue_task(struct task* task);
void dequeue_current_task_locked(void);
void task_switch(void);
void sched_start(void);
void schedule(void);
void schedule_locked(void);
void waitqueue_add_release(struct list *q, struct cs_smp_lock *lock, u32 flags);
void waitqueue_del_release(struct list *q, struct cs_smp_lock *lock, u32 cpu_flags);
void sleep_clks(u64 clks);

u64 wake_sleepers(void);
void init_background(void);
void queue_task_locked(struct task* task);
void do_task_switch(struct task *new, struct task *old);

void sem_init(struct sem* sem);
int sem_aq(struct sem* sem, u32 flags);
void sem_rel(struct sem* sem);

void cond_wait(struct cond* cond, struct cs_smp_lock *lock, u32 cpu_flags); 
void cond_signal(struct cond* cond);

#endif
