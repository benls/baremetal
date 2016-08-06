#ifndef UART_H
#define UART_H

#define UART_NONBLOCK 1

int uart_read(void *data, uint sz, uint flags);
int uart_write(const void *data, uint sz, uint flags);

void uart_init(void);
void uart_putc(char c);
char uart_getc(void);
void uart_putc_tinyprintf(void*, char c);

#endif
