#include "os.h"
#include "task.h"
#include "timer.h"

static u8 align(8) stack_a[1024];
static u8 align(8) stack_b[1024];

static struct task task_a;
static struct task task_b;

static void a(void) {
    for(int i = 0; ; i++) {
        printf("a %d\r\n", i);
        sleep_clks(us_to_clk(1000 * 1000));
    }
}

static void b(void) {
    for(int i = 0; ; i++) {
        printf("b %d\r\n", i);
        sleep_clks(us_to_clk(1000 * 1000));
    }
}

void init_print_ab(void) {
    init_task(&task_a, a, stack_a + sizeof(stack_a));
    queue_task(&task_a);
    init_task(&task_b, b, stack_b + sizeof(stack_b));
    queue_task(&task_b);
}

