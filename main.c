#include "task.h"
#include "uart.h"
#include "os.h"
#include "armv7.h"
#include "interrupt.h"
#include "timer.h"

extern char __bss_start;
extern char __bss_end;

static void disable_watchdog(void);

void init_blink(void);
void init_print_ab(void);

void init_os(void) {
    /* zero bss section */
    memset(&__bss_start, 0, &__bss_end - &__bss_start);
    disable_watchdog();
    init_printf(NULL, &uart_putc_tinyprintf);
    init_interrupt();
    timer_init();
    enable_irq(); //needed?
    init_sched();
    init_blink();
    //init_print_ab();
    printf("ttbcr %08lx\r\n", get_ttbcr());
    printf("ttbr0 %08lx\r\n", get_ttbr0());
    printf("dacr  %08lx\r\n", get_dacr());
    printf("sctlr %08lx\r\n", get_sctlr());
    sched_start(); /* Never returns */
}

#define WDT_WSPR 0x44E35048
#define WDT_WWPS 0x44E35034
#define WWPS_W_PEND_WSPR (1<<4)
static void disable_watchdog() {
    w32(WDT_WSPR, 0xaaaa);
    while(r32(WDT_WWPS) & WWPS_W_PEND_WSPR);
    w32(WDT_WSPR, 0x5555);
}

