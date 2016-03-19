#include "armv7.h"
#include "interrupt.h"
#include "os.h" /* TODO: move to debug.h */

void init_interrupt(void) {
    set_vbar(vector_table);
    intc_am335x_init();
}

