#include "armv7.h"
#include "interrupt.h"
#include "os.h" /* TODO: move to debug.h */

extern void vector_table_init(void);

void init_interrupt(void) {
    vector_table_init();
    set_vbar(vector_table);
    intc_am335x_init();
}

