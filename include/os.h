#ifndef OS_H
#define OS_H

#include <stddef.h>
#include <stdint.h>

#define cnt_of(_Array) (sizeof(_Array) / sizeof(_Array[0]))

#define container_of(ptr, type, member) __extension__({ \
        const typeof( ((type *)0)->member ) *__mptr = (ptr); \
        (type *)( (char *)__mptr - offsetof(type,member) );})

typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef int64_t s64;
typedef uint64_t u64;

typedef unsigned int uint;

#define assert(x)

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
#define memmove(x, y, z) __builtin_memmove((x), (y), (z)) 

#endif
