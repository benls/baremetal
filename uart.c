#include "os.h"
#include "uart.h"
#include "armv7.h"

#define LSR (5*4)
#define MDR1 0x20
#define IER 4
#define MCR (4*4)
#define FCR 0x8
#define THR 0
#define SYSC 0x54
#define SYSS 0x58
#define LCR 0xc
#define EFR 0x8
#define DLL 0x0
#define DLH 0x4

#define LSR_THRE 0x20
#define RXFIFOE (1<<0)
#define LSR_TEMT 0x40 
#define SYSC_NO_IDLE (1<<3)
#define SYSC_SOFTRESET 2
#define SYSS_RESETDONE 1
#define LCR_MODE_B 0xbf
#define LCR_CHARLEN_8 0x3
#define EFR_ENHANCED_EN 1

#define IERVAL 0
#define LCRVAL 3 /* 8 data, 1 stop, no parity */
#define LCR_BKSE 0x80
#define MCRVAL ( 1 | 2 ) /* MCR/DTS */
#define FCRVAL ( 1 | 2 | 4 ) 

#define DIVISOR_115200 26

/*qemu addr
* #define BASE 0x49020000
*/
/* am335x addr */
#define BASE 0x44e09000

#define uart_w32(a,v) (*(volatile unsigned long*)(BASE + (a)) = (v))
#define uart_r32(a) (*(volatile unsigned long*)(BASE + (a)))
#define uart_r16(a) (*(volatile unsigned short*)(BASE + (a)))
#define uart_w16(a,v) (*(volatile unsigned short*)(BASE + (a)) = (v))

void uart_init(void) {
    /* Soft reset */
    uart_w32(SYSC, SYSC_NO_IDLE | SYSC_SOFTRESET);
    while(!(uart_r32(SYSS) & SYSS_RESETDONE));
    uart_w32(SYSC, SYSC_NO_IDLE);
    /* switch to configuration mode B */
    uart_w32(LCR, LCR_MODE_B);
    uart_w32(EFR, EFR_ENHANCED_EN);
    /* Set baud rate */
    uart_w32(DLL, DIVISOR_115200);
    uart_w32(DLH, 0);
    /* Switch to operational mode */
    uart_w32(LCR, LCR_CHARLEN_8);
    /* Enable uart mode */
    uart_w32(MDR1, 0);
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
