#ifndef TIMER_H
#define TIMER_H

#include "os.h"

/* uboot sets this to 3mhz, could be as high as 24mhz */
#define HZ (3 * 1000 * 1000)

static inline u32 us_to_clk(u32 us) {
    u64 llus = (u64)us * (((u64)HZ<<32)/1000000);
    return (u32)(llus >> 32);
}

u32 timer_raw(void);
void wait_us(u32 us);
void wait_clk(u32 clk);
void timer_init(void);
u64 timer_get_clks(void);
void timer_sched(u32 clk);

#endif
