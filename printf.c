#include "uart.h"
#include "tinyprintf.h"
#include <stdarg.h>
#include "printf.h"

struct putc_buf {
    int acc;
    uint n;
    char buf[100];
};

static void add_to_buf(struct putc_buf *buf, char c) {
    buf->buf[buf->n++] = c;
    if (buf->n == sizeof(buf->buf)) {
        uart_write(buf->buf, sizeof(buf->buf), sizeof(buf->buf));
        buf->n = 0;
    }
}

static void putc_uart(void *arg, char c) {
    struct putc_buf *buf = arg;
    buf->acc++;
    if (c == '\n') {
        add_to_buf(buf, '\r');
        add_to_buf(buf, '\n');
    } else {
        add_to_buf(buf, c);
    }
}

int printf(const char *format, ...) {
    va_list va;
    struct putc_buf buf;

    buf.n = 0;
    buf.acc = 0;

    va_start(va, format);
    tfp_format(&buf, putc_uart, format, va);
    va_end(va);

    if (buf.n)
        uart_write(buf.buf, buf.n, buf.n);

    return buf.acc;
}

