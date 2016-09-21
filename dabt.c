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
    printf_poll("\nData abort:\n");
    printf_poll("SPSR: %08lx\n", regs->spsr);
    printf_poll("PC: %08lx\n", regs->lr_dabt - 4);
    for (uint i = 0; i < cnt_of(regs->r); i++)
        printf_poll("R%02d: %08lx\n", i, regs->r[i]);
    for(;;);
}

