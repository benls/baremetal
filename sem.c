#include "armv7.h"
#include "task.h"

void sem_init(struct sem* sem) {
    cs_smp_init(&sem->lock);
    sem->count = 0;
    sem->list = LIST_INIT(sem->list);
}

int sem_aq(struct sem* sem, u32 flags) {
    u32 cpu_flags;
    cpu_flags = cs_smp_aq(&sem->lock);
    if (sem->count == 0) {
        sem->count++;
        cs_smp_rel(&sem->lock, cpu_flags);
        return 0;
    } else if (!(flags & SEM_NONBLOCK)) {
        waitqueue_add_release(sem->list.prev, &sem->lock, cpu_flags);
        return 0;
    } else {
        cs_smp_rel(&sem->lock, cpu_flags);
        return -1;
    }
}

void sem_rel(struct sem* sem) {
    u32 cpu_flags;
    cpu_flags = cs_smp_aq(&sem->lock);
    assert(sem->count == 1);
    sem->count--;
    if (!list_empty(&sem->list)) {
        waitqueue_del_release(sem->list.next, &sem->lock, cpu_flags);
    } else {
        cs_smp_rel(&sem->lock, cpu_flags);
    }
}

