
#include "uart.h"

#define LSR (5*4)
#define MDR1 (8*4)
#define IER (1*4)
#define MCR (4*4)
#define FCR (2*4)
#define THR 0

#define LSR_THRE 0x20
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

#define w32(a,v) (*(volatile unsigned long*)(BASE + (a)) = (v))
#define r32(a) (*(volatile unsigned long*)(BASE + (a)))
#define w16(a,v) (*(volatile unsigned short*)(BASE + (a)) = (v))

void uart_init(void)
{
#if 0
    if (r32(LSR) & LSR_THRE) {
        /*set_bgr();*/
        w32(MDR1, 0);
    }

    while (!(r32(LSR) & LSR_TEMT));

    w32(IER, IERVAL);
    w32(MDR1, 0x7);
    w32(MCR, MCRVAL);
    w32(FCR, FCRVAL);

    /*set_bgr();*/

    w32(MDR1, 0x7);
#endif
}

void uart_putc(char c)
{
    while ((r32(LSR) & LSR_THRE) == 0);

    w16(THR, c);
}
