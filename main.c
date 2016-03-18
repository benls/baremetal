#include "task.h"
#include "uart.h"
#include "util.h"
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
    cpsr = get_cpsr();
    printf("CPSR... %08lx\r\n", cpsr);
    vbar = NULL;
    vbar = get_vbar();
    printf("Vbar... %p\r\n", vbar);
    init_interrupt();
    vbar = get_vbar();
    printf("Vbar... %p\r\n", vbar);
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
