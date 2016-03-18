#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "types.h"

extern u32 vector_table[5];
void init_interrupt(void);

#endif

