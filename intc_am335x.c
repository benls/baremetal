#include "interrupt.h"
#include "os.h"
#include "armv7.h"
#include "task.h"

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
#define ISR_SET(n) (0x90 + (n)*0x20)
#define ISR_CLEAR(n) (0x94 + (n)*0x20)
#define ILR(n) (0x100 + (n)*4)

#define SYSCONFIG_AUTOIDLE 1
#define SYSCONFIG_SOFTRESET 2
#define SYSSTATUS_RESET_DONE 1
#define SIR_IRQ_ACTIVE 0x7F
#define SIR_IRQ_SPURIOUS (0x1fffffful << 7)
#define CONTROL_NEWIRQ 1

#define N_IRQ 128

static struct {
    isr_t isr;
    uint flags;
} isrs[N_IRQ];

extern void vector_table_init(void);

void init_interrupt(void) {
    vector_table_init();
    set_vbar(vector_table);
    intc_am335x_init();
}

static void ack_irq() {
    intc_w(CONTROL, CONTROL_NEWIRQ);
}

void mask_irq(uint irq) {
    intc_w(MIR_SET(irq/32), 1<<(irq%32));
}

void unmask_irq(uint irq) {
    intc_w(MIR_CLEAR(irq/32), 1<<(irq%32));
}

void set_active_irq(uint irq) {
    intc_w(ISR_SET(irq/32), 1<<(irq%32));
}

void clear_active_irq(uint irq) {
    intc_w(ISR_CLEAR(irq/32), 1<<(irq%32));
}

void register_isr(isr_t isr, uint irq, uint flags) {
    isrs[irq].isr = isr;
    isrs[irq].flags = flags;
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
    //ugly hack?
    static volatile uint irq_deep;
    uint deep;
    uint irq;
    deep = irq_deep++;
    irq = intc_r(SIR_IRQ);
    if ((irq & SIR_IRQ_SPURIOUS) == SIR_IRQ_SPURIOUS)
        return;
    irq &= SIR_IRQ_ACTIVE;
    mask_irq(irq);
    ack_irq(irq);
    if (isrs[irq].isr) {
        if (!(isrs[irq].flags & ISR_FLAG_NOIRQ))
            enable_irq();
        isrs[irq].isr(irq);
    }
    unmask_irq(irq);
    //ugly hack
    disable_irq();
    irq_deep = deep;
    enable_irq();
    if (deep == 0) {
        task_switch();
    }
}

