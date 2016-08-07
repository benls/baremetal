#ifndef UART_H
#define UART_H

#include "os.h"

int uart_read(void *data, uint sz, uint need);
int uart_write(const void *data, uint sz, uint need);

void uart_init(void);
void uart_putc(char c);

#endif
