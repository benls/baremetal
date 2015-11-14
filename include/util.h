#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <types.h>

#define COUNT_OF(_Array) (sizeof(_Array) / sizeof(_Array[0]))

//TODO: why does linux use inline assembly?
#define w(a,v) w32(a,v)
#define r(a) r32(a)
#define w32(a,v) ((*(volatile u32*)(a)) = (v))
#define r32(a) (*(volatile u32*)(a))
#define w16(a,v) ((*(volatile u16*)(a)) = (v))
#define r16(a) (*(volatile u16*)(a))
#define w8(a,v) ((*(volatile u8*)(a)) = (v))
#define r8(a) (*(volatile u8*)(a))

ssize_t _write(int fd, const void *buf, size_t count);
#define debug(s) _write(2, (s), strlen(s))

#endif
