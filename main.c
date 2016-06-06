#include "task.h"
#include "uart.h"
#include "os.h"
#include "armv7.h"
#include "interrupt.h"
#include "timer.h"

extern char __bss_start;
extern char __bss_end;

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

void init_blink(void);

int main(void) {
    void* vbar;
    u32 cpsr;
    zero_bss();
    disable_watchdog();
    init_printf(NULL, &uart_putc_tinyprintf);
    debug("Starting main...\r\n");
    cpsr = get_cpsr();
    printf("CPSR... %08lx\r\n", cpsr);
    for(unsigned i =0; i < 5; i++)
        printf("Testing printf... %d\r\n", i);
    printf("Vector table: %p\r\n", (void*)vector_table);
    vbar = NULL;
    vbar = get_vbar();
    printf("Vbar... %p\r\n", vbar);
    debug("init interrupt\r\n");
    init_interrupt();
    vbar = get_vbar();
    printf("Vbar... %p\r\n", vbar);
    debug("init timer\r\n");
    timer_init();
    debug("Registering ISR...\r\n");
    register_isr(clear_irq, TEST_IRQ, 0);
    printf("ITR: %08lx\r\n", *(volatile u32*)0x48200080);
    set_active_irq(TEST_IRQ);
    printf("Software IRQ set...\r\n");
    printf("ITR: %08lx\r\n", *(volatile u32*)0x48200080);
    unmask_irq(TEST_IRQ);
    debug("Unmasked irq\r\n");
    debug("Enabling irqs...\r\n");
    enable_irq();
    printf("Returned from ISR!...\r\n");
    cpsr = get_cpsr();
    printf("CPSR... %08lx\r\n", cpsr);
    debug("sched init\r\n");
    init_sched();
    debug("start tasks\r\n");
    start_task_a();
    start_task_b();
    debug("sched_start\r\n");
    init_blink();
    sched_start();
    for(;;);   
}

static void start_task_a(void) {
    debug("init task a\r\n");
    init_task(&task_a, task_a_func, &task_a_stack[sizeof(task_a_stack) & ~0xF]);
    debug("schedule task a\r\n");
    queue_task(&task_a);
}

static void start_task_b(void) {
    init_task(&task_b, task_b_func, &task_b_stack[sizeof(task_b_stack) & ~0xF]);
    queue_task(&task_b);
}

static void task_a_func(void) {
    debug("in a\r\n");
    for(int i = 0; ; i++) {
        printf("a %d\r\n", i);
        sleep_clks(us_to_clk(1000 * 1000));
    }
}

static void task_b_func(void) {
    debug("in b\r\n");
    for(int i = 0; ; i++) {
        printf("b %d\r\n", i);
        sleep_clks(us_to_clk(1000 * 1000));
    }
}

static void zero_bss() {
    memset(&__bss_start, 0, &__bss_end - &__bss_start);
}

#define WDT_WSPR 0x44E35048
#define WDT_WWPS 0x44E35034
#define WWPS_W_PEND_WSPR (1<<4)
static void disable_watchdog() {
    w32(WDT_WSPR, 0xaaaa);
    while(r32(WDT_WWPS) & WWPS_W_PEND_WSPR);
    w32(WDT_WSPR, 0x5555);
}

