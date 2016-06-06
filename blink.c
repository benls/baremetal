#include "task.h"
#include "os.h"

static struct task blink_task;
static __attribute__((aligned(8))) u8 stack[256];

static void blink(void) {
    *((volatile u32*)0x4804C190) = 0xf<<21;
    for(;;) {
        /* GPIO1_21-GPIO1_24 */
        *((volatile u32*)0x4804C194) = 0x1<<21;
        sleep_clks(100 * 3 * 1000);
        *((volatile u32*)0x4804C190) = 0x1<<21;
        sleep_clks(3000 * 3 * 1000);
    }
}

void init_blink(void) {
    debug("init blink\r\n");
    init_task(&blink_task, blink, stack + sizeof(stack));
    queue_task(&blink_task);
}

