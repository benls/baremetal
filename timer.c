#include "os.h"
#include "timer.h"
//debug
#include <stdio.h>

/* uboot sets this to 3mhz, could be as high as 24mhz */
#define HZ (3 * 1000 * 1000)
#define BASE 0x48040000
#define TCRR 0x3c

static u32 us_to_clk(u32 us) {
    u64 llus = (u64)us * (((u64)HZ<<32)/1000000);
    return (u32)(llus >> 32);
}

u32 timer_raw(void) {
    return r32(BASE + TCRR);
}

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

