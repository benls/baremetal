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
#define TLDR 0x40

#define TCLR_ST (1<<0)
#define TCLR_AR (1<<1)
#define TCLR_CE (1<<6)
#define TCLR_PRE (1<<5)
#define TCLR_PTV_SHIFT 2
#define OVF_FLAG 2
#define MAT_FLAG 1

#define TSICR 0x54
#define TSICR_POSTED (1<<2)
#define TSICR_SFT (1<<1)

#define TWPS 0x48
#define W_PEND_TMAR (1<<4)
#define W_PEND_TTGR (1<<3)
#define W_PEND_TLDR (1<<2)
#define W_PEND_TCRR (1<<1)
#define W_PEND_TCLR (1<<0)

#define TIMER_IRQ 68

#define TIMER_INIT 0


static u64 accum_ticks;
static u32 last_tick;

u32 timer_raw(void) {
    return r32(BASE + TCRR);
}

static void wait_posted(u32 posted_bit) {
    while(r32(BASE + TWPS) & posted_bit);
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
    u32 raw;
    (void)irq;
    stat = r32(BASE + IRQ_STAT);
    if (stat & MAT_FLAG) {
        wait_posted(W_PEND_TCLR);
        w32(BASE + TCLR, r32(BASE + TCLR) & ~TCLR_CE);
    }
    w32(BASE + IRQ_STAT, stat);
    raw = timer_raw();
    accum_ticks += raw - last_tick;
    last_tick = raw;
    if (stat & MAT_FLAG)
        schedule();
}

void timer_sched(u32 clk) {
    u32 raw;
    raw = timer_raw();
    /* TODO: start/stop timer here */
    wait_posted(W_PEND_TMAR | W_PEND_TCLR);
    w32(BASE + TMAR, raw + clk);
    w32(BASE + TCLR, r32(BASE + TCLR) | TCLR_CE);
}

void timer_init(void) {
    w32(BASE + TSICR, TSICR_POSTED);
    r32(BASE + TSICR);
    w32(BASE + TSICR, TSICR_POSTED | TSICR_SFT);
    r32(BASE + TSICR);
    w32(BASE + TCRR, TIMER_INIT);
    last_tick = TIMER_INIT;
    w32(BASE + TCLR, TCLR_AR | TCLR_PRE | (2 << TCLR_PTV_SHIFT));
    w32(BASE + TLDR, 0);
    w32(BASE + IRQ_DISABLE, ~MAT_FLAG);
    w32(BASE + IRQ_STAT, OVF_FLAG | MAT_FLAG);
    w32(BASE + IRQ_ENABLE, MAT_FLAG);

    register_isr(timer_isr, TIMER_IRQ, ISR_FLAG_NOIRQ);
    unmask_irq(TIMER_IRQ);

    wait_posted(~0);
    w32(BASE + TCLR, r32(BASE + TCLR) | TCLR_ST);
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

