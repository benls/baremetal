#ifndef OS_H
#define OS_H

//TODO: move these to a new header file?
#include "tinyprintf.h"
extern void debug(const char *s);

#define cnt_of(_Array) (sizeof(_Array) / sizeof(_Array[0]))

#define offsetof(type, member)  __builtin_offsetof (type, member)

#define container_of(ptr, type, member) __extension__({ \
        const typeof( ((type *)0)->member ) *__mptr = (ptr); \
        (type *)( (char *)__mptr - offsetof(type,member) );})

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef long int s32;
typedef unsigned long int u32;
typedef long long int s64;
typedef unsigned long long int u64;

typedef unsigned int uint;
typedef unsigned int size_t;

#define NULL ((void*)0)

#define always_inline __attribute__((always_inline))
#define align(n) __attribute__((aligned(n)))

#define w32(a,v) *(volatile u32*)(a) = v
#define r32(a) (*(volatile u32*)(a))
#define w16(a,v) *(volatile u16*)(a) = v
#define r16(a) (*(volatile u16*)(a))

/* GCC builtins */
#define strlen(x) __builtin_strlen(x)
#define memcpy(x, y, z) __builtin_memcpy((x), (y), (z)) 
#define memset(x, y, z) __builtin_memset((x), (y), (z)) 

#endif
