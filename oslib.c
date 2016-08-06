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

#undef memcpy
void *memcpy(void *dest, const void *src, size_t n) {
    u8 *p = dest;
    const u8 *s = src;
    while (n--)
        *(p++) = *(s++);
    return dest;
}

#undef memmove
void *memmove(void *dest, const void *src, size_t n) {
    return memcpy(dest, src, n);
}
