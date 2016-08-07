#ifndef UART_H
#define UART_H

int uart_read(void *data, uint sz, uint need);
int uart_write(const void *data, uint sz, uint need);

void uart_init(void);
void uart_putc(char c);
char uart_getc(void);
void uart_putc_tinyprintf(void*, char c);

#endif
