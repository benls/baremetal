#ifndef TIMER_H
#define TIMER_H

#include "os.h"

u32 timer_raw(void);
void wait_us(u32 us);
void wait_clk(u32 clk);

#endif
