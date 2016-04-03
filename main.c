#include "task.h"
#include "uart.h"
#include "os.h"
#include "debug.h"
#include "armv7.h"
#include "interrupt.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

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
static void disable_watchdog(void);

void __libc_init_array();

#define TEST_IRQ 6

static void clear_irq(uint irq) {
    static int times;
    debug("IN clear_irq!\r\n");
    if (irq != TEST_IRQ) {
        debug("Unexpected irq!\r\n");
    }
    printf("cpsr %08lx\r\n", get_cpsr());
    printf("software IRQs %d... %08lx\r\n", times, *(volatile u32*)0x48200090);
    printf("ITR: %08lx\r\n", *(volatile u32*)0x48200080);
    clear_active_irq(TEST_IRQ);
    debug("Cleared!\r\n");
    if (++times > 5) {
        debug("Masking...\r\n");
        mask_irq(TEST_IRQ);
    }
    if (times > 7) {
        debug("Spinning...\r\n");
        for(;;);
    }
}

int main(void) {
    void* vbar;
    u32 cpsr;
    zero_bss();
    disable_watchdog();
    debug("Libc init array\r\n");
    __libc_init_array();
    debug("Starting main...\r\n");
    /* glibc calls sbrk with a huge number unless I do this */
    free(malloc(0));
    debug("First malloc done...\r\n");
    for(unsigned i =0; i < 5; i++)
        printf("Testing printf... %d\r\n", i);
    /* GPIO1_21-GPIO1_24 */
    *((volatile u32*)0x4804C190) = (0xf<<21);
    printf("Vector table: %p\r\n", (void*)vector_table);
    vbar = NULL;
    vbar = get_vbar();
    printf("Vbar... %p\r\n", vbar);
    init_interrupt();
    vbar = get_vbar();
    printf("Vbar... %p\r\n", vbar);
    cpsr = get_cpsr();
    printf("CPSR... %08lx\r\n", cpsr);
    register_isr(clear_irq, TEST_IRQ);
    printf("Registered ISR...\r\n");
    printf("ITR: %08lx\r\n", *(volatile u32*)0x48200080);
    set_active_irq(TEST_IRQ);
    printf("Software IRQ set...\r\n");
    printf("ITR: %08lx\r\n", *(volatile u32*)0x48200080);
    unmask_irq(TEST_IRQ);
    printf("Unmasked irq\r\n");
    printf("Enabling irqs...\r\n");
    enable_irq();
    printf("Returned from ISR!...\r\n");
    cpsr = get_cpsr();
    printf("CPSR... %08lx\r\n", cpsr);

    debug_main();
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
        printf("a\r\n");
        task_switch();
    }
}

static void task_b_func(void) {
    for(;;) {
        printf("b\r\n");
        task_switch();
    }
}

static void zero_bss() {
    for(char *c = (char*)&__bss_start; c < (char*)&__bss_end; c++)
        *c = 0;
    //why does this not work?
    //memset(&__bss_start, 0, &__bss_end - &__bss_start);
}

#define WDT_WSPR 0x44E35048
#define WDT_WWPS 0x44E35034
#define WWPS_W_PEND_WSPR (1<<4)
static void disable_watchdog() {
    w32(WDT_WSPR, 0xaaaa);
    while(r32(WDT_WWPS) & WWPS_W_PEND_WSPR);
    w32(WDT_WSPR, 0x5555);
}
