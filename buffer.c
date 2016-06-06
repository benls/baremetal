#include "buffer.h"

void buffer_init(struct buffer* b, void *data, uint sz) {
    memset(b, 0, sizeof(*b));
    b->data = data;
    b->data_sz = sz;
    sem_init(&b->read_sem);
    sem_init(&b->write_sem);


