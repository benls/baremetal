#ifndef UART_H
#define UART_H

void uart_init(void);
void uart_putc(char c);
char uart_getc(void);
void uart_putc_tinyprintf(void*, char c);

#endif
