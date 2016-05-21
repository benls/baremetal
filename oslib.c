#include "os.h"

#undef strlen

size_t strlen(const char *s) {
    size_t l = 0;
    while (*s) {
        s++;
        l++;
    }
    return l;
}

#undef memset
void *memset(void *s, int c, size_t n) {
    u8 *p = s;
    while (n--)
        *(p++) = (u8)c;
    return s;
}

