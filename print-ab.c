#include "os.h"
#include "io.h"
#include "task.h"
#include "timer.h"
#include "uart.h"

static struct task print_task;
static align(8) u8 stack[4096];

static void print_func() {
    static const char msg[] = "hello!\r\n";
    for (;;) {
        uart_write(msg, sizeof(msg) - 1, 0);
    }
}

void init_print_ab(void) {
    init_task(&print_task, print_func, stack + sizeof(stack));
    queue_task(&print_task);
}

