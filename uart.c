#include "os.h"
#include "uart.h"
#include "armv7.h"
#include "interrupt.h"

#define LSR 0x14
#define MDR1 0x20
#define IER 0x4
#define THR 0x0
#define SYSC 0x54
#define SYSS 0x58
#define LCR 0xc
#define EFR 0x8
#define DLL 0x0
#define DLH 0x4
#define SCR 0x40
#define FCR 0x8
#define MCR 0x10
#define TLR 0x1c
#define SCR 0x40
#define IIR 0x8

#define LSR_TXFIFOE (1<<5)
#define LSR_RXFIFOE (1<<0)
#define SYSC_NO_IDLE (1<<3)
#define SYSC_SOFTRESET 2
#define SYSS_RESETDONE 1
#define LCR_MODE_A 0x80
#define LCR_MODE_B 0xbf
#define LCR_CHARLEN_8 0x3
#define EFR_ENHANCED_EN 1
#define FCR_FIFO_EN (1<<0)
#define FCR_TX_TRIG_SHIFT 4
#define FCR_RX_TRIG_SHIFT 6
#define MCR_TCRTLR (1<<6)
#define TLR_RX_TRIG_SHIFT 4
#define TLR_TX_TRIG_SHIFT 0
#define SCR_TX_TRIG_GRANU1 (1<<6)
#define SCR_RX_TRIG_GRANU1 (1<<7)
#define IER_THR (1<<1)
#define IER_RHR (1<<0)
#define IIR_IT_PENDING (1<<0)
#define IIR_IT_TYPE_MASK (0x1f<<1)
#define IIR_IT_TYPE_THR (1<<1)
#define IIR_IT_TYPE_RHR (2<<1)

#define DIVISOR_115200 26

/* UART0 */
#define BASE 0x44e09000
#define IRQ 72

#define uart_w32(a,v) w32(BASE + (a),v)
#define uart_r32(a) r32(BASE + (a))
#define uart_r16(a) r16(BASE + (a))
#define uart_w16(a,v) w16(BASE + (a), v)

#define RX_TRIG 1
#define TX_TRIG 30

//debugging...
static const char * volatile tx_buf;
static volatile u32 tx_len;

static void isr(uint irq) {
    (void)irq;
    u32 iir;
    u32 type;
    iir = uart_r32(IIR);
    if (!(iir & IIR_IT_PENDING)) {
        type = iir & IIR_IT_TYPE_MASK;
        if (type == IIR_IT_TYPE_THR) {
            if (tx_len > 0) {
                uart_w16(THR, *tx_buf++);
                tx_len--;
            }
            if (tx_len == 0) {
                uart_w32(IER, uart_r32(IER) & ~IER_THR);
                tx_buf = NULL;
            }
        }
    }
}

void uart_init(void) {
    /* Soft reset */
    uart_w32(SYSC, SYSC_NO_IDLE | SYSC_SOFTRESET);
    while(!(uart_r32(SYSS) & SYSS_RESETDONE));
    uart_w32(SYSC, SYSC_NO_IDLE);
    /* switch to configuration mode B */
    uart_w32(LCR, LCR_MODE_B);
    uart_w32(EFR, EFR_ENHANCED_EN);
    /* switch to configuration mode A */
    uart_w32(LCR, LCR_MODE_A);
    /* configure FIFOs */
    uart_w32(MCR, MCR_TCRTLR);
    uart_w32(FCR, FCR_FIFO_EN |
            ((RX_TRIG & 0x3) << FCR_RX_TRIG_SHIFT) |
            ((TX_TRIG & 0x3) << FCR_TX_TRIG_SHIFT));
    uart_w32(TLR, ((RX_TRIG >> 2) << TLR_RX_TRIG_SHIFT) |
            ((TX_TRIG >> 2) << TLR_TX_TRIG_SHIFT));
    uart_w32(SCR, SCR_TX_TRIG_GRANU1 | SCR_RX_TRIG_GRANU1);
    /* Set baud rate */
    uart_w32(DLL, DIVISOR_115200);
    uart_w32(DLH, 0);
    /* Switch to operational mode */
    uart_w32(LCR, LCR_CHARLEN_8);
    //uart_w32(IER, IER_THR/* | IER_RHR*/);
    /* Enable uart mode */
    uart_w32(MDR1, 0);

    register_isr(isr, IRQ, ISR_FLAG_NOIRQ);
    unmask_irq(IRQ);
}

void debug(const char* s) {
    uint l = strlen(s);
    for(uint i = 0; i < l; i++)
        uart_putc(s[i]);
}

void uart_putc(char c) {
    //debugging...
    u32 flags;
    while(tx_len != 0 && tx_buf != NULL);
    tx_buf = &c;
    tx_len = 1;
    flags = disable_irq();
    uart_w32(IER, uart_r32(IER) | IER_THR);
    set_cpsr(flags);
    while(tx_len != 0 && tx_buf != NULL);
}

void uart_putc_tinyprintf(void* unused, char c) {
    (void)unused;
    uart_putc(c);
}

char uart_getc(void) {
    //TODO: is this broken now?
    char c;
    u32 flags;
    //TODO: use real locks
    flags = disable_irq();
    while (!(uart_r32(LSR) & LSR_RXFIFOE));

    c = (char)(uart_r16(THR) & 0xff);
    set_cpsr(flags);
    return c;
}
