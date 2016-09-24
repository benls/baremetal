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
	asm volatile("mcr p15, 0, %0, c12, c0, 0" : : "r" (val) : "memory");
}

static inline u32 get_cpsr(void) {
    u32 cpsr;
    asm volatile("mrs %0, cpsr" : "=r" (cpsr));
    return cpsr;
}

static inline void set_cpsr(u32 cpsr) {
    asm volatile("msr cpsr, %0" : : "r" (cpsr) : "memory");
}

static inline u32 get_spsr(void) {
    u32 spsr;
    asm volatile("mrs %0, spsr" : "=r" (spsr));
    return spsr;
}

static inline void set_spsr(u32 cpsr) {
    asm volatile("msr spsr, %0" : : "r" (cpsr) : "memory");
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
	asm volatile("mcr p15, 0, %0, c2, c0, 2" : : "r" (val) : "memory");
}


static inline u32 get_ttbr0(void) {
	u32 val;
	asm volatile("mrc p15, 0, %0, c2, c0, 0" : "=r" (val));
	return val;
}

static inline void set_ttbr0(u32 val) {
	asm volatile("mcr p15, 0, %0, c2, c0, 0" : : "r" (val) : "memory");
}

static inline u32 get_dacr(void) {
	u32 val;
	asm volatile("mrc p15, 0, %0, c3, c0, 0" : "=r" (val));
	return val;
}

static inline void set_dacr(u32 val) {
	asm volatile("mcr p15, 0, %0, c3, c0, 0" : : "r" (val) : "memory");
}

static inline u32 get_sctlr(void) {
	u32 val;
	asm volatile("mrc p15, 0, %0, c1, c0, 0" : "=r" (val));
	return val;
}

static inline void set_sctlr(u32 val) {
	asm volatile("mcr p15, 0, %0, c1, c0, 0" : : "r" (val) : "memory");
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
struct cs_smp_lock {
    //TODO: SMP
};
#pragma GCC diagnostic pop

static inline u32 cs_smp_aq(struct cs_smp_lock *lock) {
    //TODO: SMP
    (void)lock;
    return disable_irq();
}

static inline void cs_smp_rel(struct cs_smp_lock *lock, u32 cpu_flags) {
    //TODO: SMP
    (void)lock;
    set_cpsr(cpu_flags);
}

static inline void cs_smp_init(struct cs_smp_lock *lock){
    (void)lock;
}

static inline int syscall_1(uint syscall_nr, int a) {
    register int a1 asm("r0") = a;
    register int nr asm("r7") = syscall_nr;
    asm volatile("svc 0" : "=r" (a1) : "r" (nr), "r" (a1) : "memory");
    return a1;
}

//just for debugging
static inline int syscall_1_from_svc(uint syscall_nr, int a) {
    int spsr;
    int lr;
    int ret;

    spsr = get_spsr();
	asm volatile("mov %0, lr" : "=r" (lr));

    ret = syscall_1(syscall_nr, a);

    set_spsr(spsr);
    asm volatile("mov lr, %0" : : "r" (lr));

    return ret;
}

#endif

