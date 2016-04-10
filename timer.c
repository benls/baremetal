#include "os.h"
#include "timer.h"
#include "armv7.h"
#include "interrupt.h"
#include "task.h"

#define BASE 0x48040000
#define TCLR 0x38
#define TCRR 0x3c
#define IRQ_ENABLE 0x2c
#define IRQ_DISABLE 0x30
#define IRQ_STAT 0x28
#define TMAR 0x4c

#define TCLR_AR (1<<1)
#define TCLR_CE (1<<6)
#define OVF_FLAG 2
#define MAT_FLAG 1

#define TIMER_IRQ 68

static u64 accum_ticks;
static u32 last_tick;

u32 timer_raw(void) {
    return r32(BASE + TCRR);
}

u64 timer_get_clks(void) {
    u64 ret;
    u32 flags;
    flags = disable_irq();
    ret = accum_ticks + (timer_raw() - last_tick);
    set_cpsr(flags);
    return ret;
}

static void timer_isr(uint irq) {
    u32 stat;
    (void)irq;
    stat = r32(BASE + IRQ_STAT);
    if (stat & OVF_FLAG)
        accum_ticks += 0x100000000ull;
    if (stat & MAT_FLAG) {
        w32(BASE + TCLR, r32(BASE + TCLR) & ~TCLR_CE);
    }
    w32(BASE + IRQ_STAT, stat);
    last_tick = timer_raw();
    schedule();
}

void timer_sched(u32 clk) {
    /* Could start/stop timer here to be safe */
    w32(BASE + TMAR, timer_raw() + clk);
    w32(BASE + TCLR, r32(BASE + TCLR) | TCLR_CE);
}

void timer_init(void) {
    /* Timer is configured by uboot */
    w32(BASE + TCLR, r32(BASE + TCLR) & ~(TCLR_AR | TCLR_CE));
    w32(BASE + IRQ_DISABLE, ~(OVF_FLAG | MAT_FLAG));
    w32(BASE + IRQ_STAT, OVF_FLAG | MAT_FLAG);
    w32(BASE + IRQ_ENABLE, OVF_FLAG | MAT_FLAG);
    register_isr(timer_isr, TIMER_IRQ, ISR_FLAG_NOIRQ);
    unmask_irq(TIMER_IRQ);
}

//inline?
void wait_us(u32 us) {
    u32 clk;
    clk = us_to_clk(us);
    if (us != 0)
        clk += 1;
    wait_clk(clk);
}

void wait_clk(u32 clk) {
    u32 timer;
    u32 timer_prev;
    u32 waited;
    waited = 0;
    timer_prev = timer_raw();
    while (waited < clk) {
        //TODO: spin here to be nice to l3 bus
        timer = timer_raw();
        waited += timer - timer_prev;
        timer_prev = timer;
    }
}

