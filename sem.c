#include "armv7.h"
#include "task.h"

void sem_init(struct sem* sem) {
    sem->count = 0;
    sem->list = LIST_INIT(sem->list);
}

void sem_aq(struct sem* sem) {
    u32 flags;
    flags = disable_irq();
    if (sem->count == 0) {
        sem->count++;
        set_cpsr(flags);
    } else {
        waitqueue_add(sem->list.prev, flags);
    }
}

void sem_rel(struct sem* sem) {
    u32 flags;
    struct task *t;
    flags = disable_irq();
    sem->count--;
    if (!list_empty(&sem->list)) {
        //TODO: assert count > 0
        t = container_of(sem->list.next, struct task, q);
        list_del(&t->q);
        //TODO: lock task
        queue_task_locked(t);
        //TODO: schedule
    }
    set_cpsr(flags);
}

