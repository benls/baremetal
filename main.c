#include "task.h"
#include "uart.h"
#include "os.h"
#include "armv7.h"
#include "interrupt.h"
#include "timer.h"
#include "pages.h"

extern char __bss_start;
extern char __bss_end;

static void disable_watchdog(void);

static void stage2(void);
static struct task stage2_task;
static align(8) u8 stage2_stack[4096];

void init_blink(void);
void init_print_ab(void);

void init_os(void) {
    /* zero bss section */
    memset(&__bss_start, 0, &__bss_end - &__bss_start);
    disable_watchdog();
    init_interrupt();
    timer_init();
    uart_init();
    init_sched();
    init_task(&stage2_task,
            stage2,
            stage2_stack + sizeof(stage2_stack));
    queue_task(&stage2_task);
    sched_start(); /* Never returns */
}

static void stage2(void) {
    init_pages();
    init_blink();
    init_print_ab();
}

#define WDT_WSPR 0x44E35048
#define WDT_WWPS 0x44E35034
#define WWPS_W_PEND_WSPR (1<<4)
static void disable_watchdog() {
    w32(WDT_WSPR, 0xaaaa);
    while(r32(WDT_WWPS) & WWPS_W_PEND_WSPR);
    w32(WDT_WSPR, 0x5555);
}

