#include "interrupt.h"
#include "os.h"

#define INTC_BASE 0x48200000
#define intc_r(a) r((a)+INTC_BASE)
#define intc_w(a,v) w((a)+INTC_BASE,(v))

#define SYSCONFIG 0x10
#define SYSSTATUS 0x14
#define SIR_IRQ 0x40
#define CONTROL 0x48
#define MIR(n) (0x84 + (n)*0x20)
#define MIR_CLEAR(n) (0x88 + (n)*0x20)
#define MIR_SET(n) (0x8C + (n)*0x20)
#define MIR_SET(n) (0x8C + (n)*0x20)
#define ILR(n) (0x100 + (n)*4)

#define SYSCONFIG_AUTOIDLE 1
#define SYSCONFIG_SOFTRESET 2
#define SYSSTATUS_RESET_DONE 1
#define SIR_IRQ_ACTIVE 0x7F
#define SIR_IRQ_SPURIOUS (0x1fffffful << 7)
#define CONTROL_NEWIRQ 1

#define N_IRQ 128

static isr_t isr[N_IRQ];

static void ack_irq(void) {
    intc_w(CONTROL, CONTROL_NEWIRQ);
}

void mask_irq(uint irq) {
    intc_w(MIR_SET(irq/32), 1<<(irq%32));
}

void unmask_irq(uint irq) {
    intc_w(MIR_CLEAR(irq/32), 1<<(irq%32));
}

void intc_am335x_init(void) {
    /* Reset peripheral */
    intc_w(SYSCONFIG, SYSCONFIG_SOFTRESET);
    while (!(intc_r(SYSSTATUS) & SYSSTATUS_RESET_DONE));
    /* Set all interrupts to IRQ, zero priority */
    for (int i = 0; i < N_IRQ; i++)
        intc_w(ILR(i), 0);
    /* Mask all interrupts */
    for (int i = 0; i < 4; i++)
        intc_w(MIR(i), ~0);
    ack_irq();
}

void irq_handler(void) {
    uint irq;
    uint active;
    debug("IN IRQ HANDLER!");
    irq = intc_r(SIR_IRQ);
    active = irq & SIR_IRQ_ACTIVE;
    /* from linux */
    if ((irq & SIR_IRQ_SPURIOUS) == SIR_IRQ_SPURIOUS || !isr[active]) {
        ack_irq();
        return;
    }
    /* call ISR */
    isr[active](active);
    /* TODO: ack here? */
    /* TODO: implement irq return */
    for(;;);
}

