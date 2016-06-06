#ifndef BUFFER_H
#define BUFFER_H

#include "task.h"
#include "os.h"

struct buffer {
    u8 *data;
    uint data_sz;
    u8 *read_head;
    u8 *write_head;
    struct sem read_sem;
    struct sem write_sem;
    struct cond read_cond;
    struct cond write_cond;
    uint read_req;
    uint write_req;
    /* called with buffer locked */
    void *priv;
    void (*read_available_prime)(void*);
    void (*write_available_prime)(void*);
    //TODO: spinlock
};

#define BUFFER_NONBLOCKING 1
void buffer_init(struct buffer* b, void* data, uint sz);

/* sleeping */
int buffer_write(struct buffer* b, const void *data, uint sz, uint flags);
int buffer_read(struct buffer* b, void *data, uint sz, uint flags);
/* non-sleeping */
u32 buffer_lock(struct buffer* b);
void buffer_unlock(struct buffer* b, u32 flags);
int buffer_read_locked(struct buffer* b, void *data, uint sz);
int buffer_write_locked(struct buffer* b, const void *data, uint sz);

#endif
