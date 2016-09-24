#include "os.h"
#include "printf.h"
#include "task.h"
#include "timer.h"
#include "uart.h"

static struct task print_task;
static align(8) u8 stack[4096];

static void print_func() {
    int x;
    printf("ttbcr %08lx\r\n", get_ttbcr());
    printf("ttbr0 %08lx\r\n", get_ttbr0());
    printf("dacr  %08lx\r\n", get_dacr());
    printf("sctlr %08lx\r\n", get_sctlr());
    printf("Making a syscall...\n");
    x = syscall_1_from_svc(0, 1234);
    printf("...result: %d\n", x);

    for (;;) {
        printf("tick");
        sleep_clks(5000 * 1000);
        printf(" tock\n");
        sleep_clks(5000 * 1000);
    }
}

void init_print_ab(void) {
    init_task(&print_task, print_func, stack + sizeof(stack));
    queue_task(&print_task);
}

