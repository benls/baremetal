#include "armv7.h"
#include "interrupt.h"
#include "util.h" /* TODO: move to debug.h */

void init_interrupt(void) {
    set_vbar(vector_table);
}

void irq_handler(void) {
    debug("IN IRQ HANDLER!");
    /* TODO: implement irq return */
    for(;;);
}
