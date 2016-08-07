#include "os.h"
#include "io.h"
#include "task.h"
#include "timer.h"
#include "uart.h"

static struct task print_task;
static align(8) u8 stack[4096];

static void print_func() {
    int sz;
    char buf[100];
    for (;;) {
        sleep_clks(1000 * 3 * 1000);
        sz = uart_read(buf, sizeof(buf), 1);
        if (sz > 0) {
            uart_write(buf, sz, sz);
        }
    }
}

void init_print_ab(void) {
    init_task(&print_task, print_func, stack + sizeof(stack));
    queue_task(&print_task);
}

