#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "os.h"

void init_interrupt(void);
typedef void (*isr_t)(uint irq);

void register_isr(isr_t isr, uint irq);

/* arch specific */
extern u32 vector_table[5];
void irq_handler(void);
void intc_am335x_init(void);
void unmask_irq(uint irq);
void mask_irq(uint irq);

void set_active_irq(uint irq);
void clear_active_irq(uint irq);

#endif

