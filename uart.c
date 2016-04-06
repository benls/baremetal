#include "os.h"
#include "uart.h"
#include "armv7.h"

#define LSR (5*4)
#define MDR1 (8*4)
#define IER (1*4)
#define MCR (4*4)
#define FCR (2*4)
#define THR 0

#define LSR_THRE 0x20
#define RXFIFOE (1<<0)
#define LSR_TEMT 0x40 

#define IERVAL 0
#define LCRVAL 3 /* 8 data, 1 stop, no parity */
#define LCR_BKSE 0x80
#define MCRVAL ( 1 | 2 ) /* MCR/DTS */
#define FCRVAL ( 1 | 2 | 4 ) 

/*qemu addr
* #define BASE 0x49020000
*/
/* am335x addr */
#define BASE 0x44e09000

#define uart_w32(a,v) (*(volatile unsigned long*)(BASE + (a)) = (v))
#define uart_r32(a) (*(volatile unsigned long*)(BASE + (a)))
#define uart_r16(a) (*(volatile unsigned short*)(BASE + (a)))
#define uart_w16(a,v) (*(volatile unsigned short*)(BASE + (a)) = (v))

void uart_init(void)
{
#if 0
    if (uart_r32(LSR) & LSR_THRE) {
        /*set_bgr();*/
        uart_w32(MDR1, 0);
    }

    while (!(uart_r32(LSR) & LSR_TEMT));

    uart_w32(IER, IERVAL);
    uart_w32(MDR1, 0x7);
    uart_w32(MCR, MCRVAL);
    uart_w32(FCR, FCRVAL);

    /*set_bgr();*/

    uart_w32(MDR1, 0x7);
#endif
}

void debug(const char* s) {
    uint l = __builtin_strlen(s);
    for(uint i = 0; i < l; i++)
        uart_putc(s[i]);
}

void uart_putc(char c)
{
    u32 flags;
    //TODO: use real locks
    flags = disable_irq();
    while ((uart_r32(LSR) & LSR_THRE) == 0);

    uart_w16(THR, c);
    set_cpsr(flags);
}

void uart_putc_tinyprintf(void* unused, char c) {
    (void)unused;
    uart_putc(c);
}

char uart_getc(void)
{
    char c;
    u32 flags;
    //TODO: use real locks
    flags = disable_irq();
    while (!(uart_r32(LSR) & RXFIFOE));

    c = (char)(uart_r16(THR) & 0xff);
    set_cpsr(flags);
    return c;
}
