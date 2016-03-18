#ifndef ARMV7_H
#define ARMV7_H

#include "types.h"

#define PSR_I_BIT (1<<7)

static inline void *get_vbar(void)
{
	void *val;
	asm("mrc p15, 0, %0, c12, c0, 0" : "=r" (val));
	return val;
}

static inline void set_vbar(void* val)
{
	asm volatile("mcr p15, 0, %0, c12, c0, 0"
	  : : "r" (val));
}

static inline u32 get_cpsr(void) {
    u32 cpsr;
    asm("mrs %0, cpsr" : "=r" (cpsr));
    return cpsr;
}

static inline void set_cpsr(u32 cpsr) {
    asm volatile("msr cpsr, %0" : : "r" (cpsr));
}

static inline u32 disable_irq(void) {
    u32 cpsr;
    cpsr = get_cpsr();
    set_cpsr(cpsr | PSR_I_BIT);
    return cpsr;
}

static inline u32 enable_irq(void) {
    u32 cpsr;
    cpsr = get_cpsr();
    set_cpsr(cpsr & ~PSR_I_BIT);
    return cpsr;
}

#endif
