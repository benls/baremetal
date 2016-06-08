#ifndef ARMV7_H
#define ARMV7_H

#include "os.h"

#define PSR_I_BIT (1<<7)

static inline void* get_vbar(void) {
	void *val;
	asm volatile("mrc p15, 0, %0, c12, c0, 0" : "=r" (val));
	return val;
}

static inline void set_vbar(void* val) {
	asm volatile("mcr p15, 0, %0, c12, c0, 0" : : "r" (val));
}

static inline u32 get_cpsr(void) {
    u32 cpsr;
    asm volatile("mrs %0, cpsr" : "=r" (cpsr));
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

/* MMU */
static inline u32 get_ttbcr(void) {
	u32 val;
	asm volatile("mrc p15, 0, %0, c2, c0, 2" : "=r" (val));
	return val;
}

static inline void set_ttbcr(u32 val) {
	asm volatile("mcr p15, 0, %0, c2, c0, 2" : : "r" (val));
}


static inline u32 get_ttbr0(void) {
	u32 val;
	asm volatile("mrc p15, 0, %0, c2, c0, 0" : "=r" (val));
	return val;
}

static inline void set_ttbr0(u32 val) {
	asm volatile("mcr p15, 0, %0, c2, c0, 0" : : "r" (val));
}

static inline u32 get_dacr(void) {
	u32 val;
	asm volatile("mrc p15, 0, %0, c3, c0, 0" : "=r" (val));
	return val;
}

static inline void set_dacr(u32 val) {
	asm volatile("mcr p15, 0, %0, c3, c0, 0" : : "r" (val));
}

static inline u32 get_sctlr(void) {
	u32 val;
	asm volatile("mrc p15, 0, %0, c1, c0, 0" : "=r" (val));
	return val;
}

static inline void set_sctlr(u32 val) {
	asm volatile("mcr p15, 0, %0, c1, c0, 0" : : "r" (val));
}

#endif

