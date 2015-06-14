#include "task.h"
#include "uart.h"

extern u32 __bss_start;
extern u32 __bss_end;

static u8 task_a_stack[1024] __attribute__((aligned(8)));
static struct task task_a;
static void task_a_func(void);
static u8 task_b_stack[1024] __attribute__((aligned(8)));
static struct task task_b;
static void task_b_func(void);
static void zero_bss(void);
static void start_task_a(void);
static void start_task_b(void);

void main(void) {
    zero_bss(); // may screw up stack
    uart_init();
    start_task_a();
    start_task_b();
    for(;;)
        task_switch();
}

static void start_task_a(void) {
    new_task(&task_a, task_a_func, &task_a_stack[sizeof(task_a_stack) & ~0xF]);
    queue_task(&task_a);
}

static void start_task_b(void) {
    new_task(&task_b, task_b_func, &task_b_stack[sizeof(task_b_stack) & ~0xF]);
    queue_task(&task_b);
}
static void task_a_func(void) {
    for(;;) {
        uart_putc('a');
        task_switch();
    }
}

static void task_b_func(void) {
    for(;;) {
        uart_putc('b');
        task_switch();
    }
}
static void zero_bss() {
    u32 *i = &__bss_start;
    while (i < &__bss_end)
        *i++ = 0;
}
