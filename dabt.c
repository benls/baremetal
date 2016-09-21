#include "task.h"
#include "os.h"
#include "printf.h"

struct dabt_regs {
    u32 r[13];
    u32 lr_dabt;
    u32 spsr;
};

void dabt_handler(struct dabt_regs* regs) {
    (void)regs;
    printf_poll("\ndata abort\n");
    for(;;);
}

