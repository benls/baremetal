#include "task.h"
#include "os.h"

static struct task blink_task;
static align(8) u8 stack[256];

static void blink(void) {
    w32(0x4804C190, 0xf<<21);
    for(;;) {
        /* GPIO1_21-GPIO1_24 */
        w32(0x4804C194, 0x1<<21);
        sleep_clks(100 * 3 * 1000);
        w32(0x4804C190, 0x1<<21);
        sleep_clks(3000 * 3 * 1000);
    }
}

void init_blink(void) {
    init_task(&blink_task, blink, stack + sizeof(stack));
    queue_task(&blink_task);
}

