#include "os.h"
#include "printf.h"
#include "task.h"
#include "timer.h"
#include "uart.h"

static struct task print_task;
static align(8) u8 stack[4096];

static void print_func() {
    printf("ttbcr %08lx\r\n", get_ttbcr());
    printf("ttbr0 %08lx\r\n", get_ttbr0());
    printf("dacr  %08lx\r\n", get_dacr());
    printf("sctlr %08lx\r\n", get_sctlr());

    for (;;) {
        uart_write("Hello!\r\n", 8, 8);
    }
}

void init_print_ab(void) {
    init_task(&print_task, print_func, stack + sizeof(stack));
    queue_task(&print_task);
}

