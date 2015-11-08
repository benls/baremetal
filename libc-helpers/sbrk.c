#include <unistd.h>

extern char __end;
static char *heap_limit;

void *_sbrk(intptr_t delta)
{
    char * prev;

    if (heap_limit == NULL)
        heap_limit = &__end;

    prev = heap_limit;
    heap_limit += delta;
    return prev;
}

