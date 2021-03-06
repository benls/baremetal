#include "os.h"
#include "uart.h"
#include "armv7.h"
#include "interrupt.h"
#include "task.h"

#define BUF_SZ 1024
//TODO: Move omap uart to a seperate file
struct buffer {
    struct cs_smp_lock spinlock;
    u8 read_buf[BUF_SZ];
    u8 write_buf[BUF_SZ];
    uint read_buf_cnt;
    uint write_buf_cnt;
    struct sem read_sem;
    struct sem write_sem;
    struct cond read_cond;
    struct cond write_cond;
    uint read_req;
    uint write_req;
};

static struct buffer b;

static uint buffer_remove_atomic(struct buffer* b, void *data, uint sz);
static uint buffer_insert_atomic(struct buffer* b, const void *data, uint sz);

int uart_write(const void *data, uint sz, uint need) {
    int err;
    const u8 *out;
    u32 cpu_flags;
    uint nwrite;
    uint txed;
    out = data;
    nwrite = 0;
    if (need > sz)
        return -1;
    if (!sz)
        return 0;
    err = sem_aq(&b.write_sem, need ? SEM_NONBLOCK : 0);
    if (err)
        return -1;
    for(;;) {
        cpu_flags = cs_smp_aq(&b.spinlock);
        txed = buffer_insert_atomic(&b, out, sz);
        out += txed;
        sz -= txed;
        need = txed > need ? 0 : need - txed;
        nwrite += txed;
        if (need) {
            b.write_req = need > sizeof(b.write_buf) ? sizeof(b.write_buf) : need;
            cond_wait(&b.write_cond, &b.spinlock, cpu_flags);
        } else {
            cs_smp_rel(&b.spinlock, cpu_flags);
            break;
        }
    }
    sem_rel(&b.write_sem);
    return (int)nwrite;
}

int uart_read(void *data, uint sz, uint need) {
    int err;
    u8 *out;
    u32 cpu_flags;
    uint nread;
    uint rxed;
    out = data;
    nread = 0;
    if (need > sz)
        return -1;
    if (!sz)
        return 0;
    err = sem_aq(&b.read_sem, need ? SEM_NONBLOCK : 0);
    if (err)
        return -1;
    for(;;) {
        cpu_flags = cs_smp_aq(&b.spinlock);
        rxed = buffer_remove_atomic(&b, out, sz);
        out += rxed;
        sz -= rxed;
        need = rxed > need ? 0 : need - rxed;
        nread += rxed;
        if (need) {
            b.read_req = need > sizeof(b.read_buf) ? sizeof(b.read_buf) : need;
            cond_wait(&b.read_cond, &b.spinlock, cpu_flags);
        } else {
            cs_smp_rel(&b.spinlock, cpu_flags);
            break;
        }
    }
    sem_rel(&b.read_sem);
    return (int)nread;
}


static void omap_uart_init();

void uart_init(void) {
    memset(&b, 0, sizeof(b));
    b.read_buf_cnt = 0;
    b.write_buf_cnt = 0;
    cs_smp_init(&b.spinlock);
    sem_init(&b.read_sem);
    sem_init(&b.write_sem);
    omap_uart_init();
}

#define THR 0x0
#define RHR 0x0
#define DLL 0x0
#define IER 0x4
#define DLH 0x4
#define EFR 0x8
#define FCR 0x8
#define IIR 0x8
#define LCR 0xc
#define MCR 0x10
#define LSR 0x14
#define TLR 0x1c
#define MDR1 0x20
#define SCR 0x40
#define SCR 0x40
#define SYSC 0x54
#define SYSS 0x58
#define RXFIFO_LVL 0x64
#define TXFIFO_LVL 0x68

#define LSR_RXFIFOE (1<<0)
#define LSR_TXFIFOE (1<<5)
#define LSR_TXSRE (1<<6)
#define SYSC_NO_IDLE (1<<3)
#define SYSC_SOFTRESET 2
#define SYSS_RESETDONE 1
#define LCR_MODE_A 0x80
#define LCR_MODE_B 0xbf
#define LCR_CHARLEN_8 0x3
#define EFR_ENHANCED_EN (1<<4)
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

#define uart_r16(a) r16(BASE + (a))
#define uart_w16(a,v) w16(BASE + (a), v)

#define RX_TRIG 1
#define TX_TRIG 30
#define TX_FIFO_SZ 64
//TODO: -10
#define TX_SZ (TX_FIFO_SZ - TX_TRIG - 10)

static uint buffer_remove_atomic(struct buffer* b, void *data, uint sz){
    sz = sz > b->read_buf_cnt ? b->read_buf_cnt : sz;
    if (!sz)
        return 0;
    memcpy(data, b->read_buf, sz);
    memmove(b->read_buf, &b->read_buf[sz], b->read_buf_cnt - sz);
    b->read_buf_cnt -= sz;
    /* enable RX interrupt */
    if (b->read_buf_cnt != sizeof(b->read_buf))
        uart_w16(IER, uart_r16(IER) | IER_RHR);
    return sz;
}

static uint buffer_insert_atomic(struct buffer* b, const void *data, uint sz) {
    u32 write_avail;
    write_avail = sizeof(b->write_buf) - b->write_buf_cnt;
    sz = sz > write_avail ? write_avail : sz;
    if (!sz)
        return 0;
    memcpy(&b->write_buf[b->write_buf_cnt], data, sz);
    b->write_buf_cnt += sz;
    uart_w16(IER, uart_r16(IER) | IER_THR);
    return sz;
}

static inline void tx_isr(void) {
    uint i;
    uint sz;
    sz = b.write_buf_cnt > TX_SZ ? TX_SZ : b.write_buf_cnt;
    for (i = 0; i < sz; i++) {
        uart_w16(THR, b.write_buf[i]);
    }
    b.write_buf_cnt -= sz;
    memmove(&b.write_buf[0], &b.write_buf[sz], b.write_buf_cnt);

    /* Disable interrupt when TX buffer is empty */
    if (b.write_buf_cnt == 0)
        uart_w16(IER, uart_r16(IER) & ~IER_THR);

    /* Wake writer */
    if (b.write_req && sizeof(b.write_buf) - b.write_buf_cnt >= b.write_req)
        cond_signal(&b.write_cond);
}

static inline void rx_isr(void) {
    uint available;
    uint rxfifo_lvl;
    uint sz;

    available = sizeof(b.read_buf) - b.read_buf_cnt;
    rxfifo_lvl = uart_r16(RXFIFO_LVL) & 0xff;
    sz = available > rxfifo_lvl ? rxfifo_lvl : available;
    while (sz--)
        b.read_buf[b.read_buf_cnt++] = uart_r16(RHR);

    /* Disable interrupt when RX buffer is full */
    if (b.read_buf_cnt == sizeof(b.read_buf))
        uart_w16(IER, uart_r16(IER) & ~IER_RHR);

    /* Wake reader */
    if (b.read_req && b.read_buf_cnt >= b.read_req) {
        cond_signal(&b.read_cond);
    }
}

static void isr(uint irq) {
    (void)irq;
    u32 iir;
    u32 type;
    for(;;) {
        iir = uart_r16(IIR);
        if (iir & IIR_IT_PENDING) {
            break;
        } else {
            type = iir & IIR_IT_TYPE_MASK;
            if (type == IIR_IT_TYPE_THR)
                tx_isr();
            else if (type == IIR_IT_TYPE_RHR)
                rx_isr();
            else
               break;
        }
    }
}

static void omap_uart_init(void) {
    /* Soft reset */
    uart_w16(SYSC, SYSC_NO_IDLE | SYSC_SOFTRESET);
    while(!(uart_r16(SYSS) & SYSS_RESETDONE));
    uart_w16(SYSC, SYSC_NO_IDLE);
    /* switch to configuration mode B */
    uart_w16(LCR, LCR_MODE_B);
    uart_w16(EFR, EFR_ENHANCED_EN);
    /* switch to configuration mode A */
    uart_w16(LCR, LCR_MODE_A);
    /* configure FIFOs */
    uart_w16(MCR, MCR_TCRTLR);
    uart_w16(TLR, ((RX_TRIG >> 2) << TLR_RX_TRIG_SHIFT) |
                  ((TX_TRIG >> 2) << TLR_TX_TRIG_SHIFT));
    uart_w16(FCR, FCR_FIFO_EN |
            ((RX_TRIG & 0x3) << FCR_RX_TRIG_SHIFT) |
            ((TX_TRIG & 0x3) << FCR_TX_TRIG_SHIFT));
    uart_w16(SCR, SCR_RX_TRIG_GRANU1 | SCR_TX_TRIG_GRANU1);
    /* Set baud rate */
    uart_w16(DLL, DIVISOR_115200);
    uart_w16(DLH, 0);
    /* Switch to operational mode */
    uart_w16(LCR, LCR_CHARLEN_8);
    /* Enable uart mode */
    uart_w16(MDR1, 0);
    uart_w16(IER, IER_RHR);

    register_isr(isr, IRQ, ISR_FLAG_NOIRQ);
    unmask_irq(IRQ);
}

void uart_putc(char c) {
    u32 flags;
    flags = disable_irq();
    while ((uart_r16(LSR) & LSR_TXSRE) == 0);
    uart_w16(THR, c);
    while ((uart_r16(LSR) & LSR_TXSRE) == 0);
    set_cpsr(flags);
}

