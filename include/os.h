#ifndef OS_H
#define OS_H

//TODO: move these to a new header file?
#include "tinyprintf.h"
extern void debug(const char *s);

#define COUNT_OF(_Array) (sizeof(_Array) / sizeof(_Array[0]))

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

//TODO: why does linux use inline assembly?
#define w(a,v) w32(a,v)
#define r(a) r32(a)
#define w32(a,v) ((*(volatile u32*)(a)) = (v))
#define r32(a) (*(volatile u32*)(a))
#define w16(a,v) ((*(volatile u16*)(a)) = (v))
#define r16(a) (*(volatile u16*)(a))
#define w8(a,v) ((*(volatile u8*)(a)) = (v))
#define r8(a) (*(volatile u8*)(a))

/* GCC builtins */
#define strlen(x) __builtin_strlen(x)
#define memcpy(x, y, z) __builtin_memcpy((x), (y), (z)) 
#define memset(x, y, z) __builtin_memset((x), (y), (z)) 

#endif