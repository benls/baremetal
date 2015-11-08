#include "task.h"
#include "uart.h"
#include "debug.h"

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

void __libc_init_array();

int main(void) {
    zero_bss();
    write(1, "Libc init array\r\n", strlen("Libc init array\r\n"));
    __libc_init_array();
    write(1, "Starting main...\r\n", strlen("Starting main...\r\n"));
    /* glibc calls sbrk with a huge number unless I do this */
    free(malloc(0));
    write(1, "First malloc done...\r\n", strlen("First malloc done...\r\n"));
    for(unsigned i =0; i < 5; i++)
        printf("Testing printf... %d\r\n", i);
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
