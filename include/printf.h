#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>

int printf(const char *format, ...)
    __attribute__ ((format (printf, 1, 2)));

#endif
