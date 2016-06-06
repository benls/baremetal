#include "task.h"

static align(8) u8 stack[256];
static struct task background_task;

static void background(void) {
    for(;;)
        asm volatile("wfe" : :);
}

void init_background(void) {
    init_task(&background_task,
            background,
            stack + sizeof(stack));
    background_task.priority = N_PRIORITY - 1;
    queue_task(&background_task);
}

