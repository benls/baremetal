#include "os.h"
#include "printf.h"
#include "task.h"
#include "timer.h"
#include "uart.h"

static struct task print_task;
static align(8) u8 stack[4096];

static void print_func() {
    int sz;
    char buf[100];

    printf("ttbcr %08lx\r\n", get_ttbcr());
    printf("ttbr0 %08lx\r\n", get_ttbr0());
    printf("dacr  %08lx\r\n", get_dacr());
    printf("sctlr %08lx\r\n", get_sctlr());

    for (;;) {
        sz = uart_read(buf, sizeof(buf), 1);
        if (sz > 0) {
            if (buf[0] == 'z')
                r32(0);
            uart_write(buf, sz, sz);
        }
    }
}

void init_print_ab(void) {
    init_task(&print_task, print_func, stack + sizeof(stack));
    queue_task(&print_task);
}

