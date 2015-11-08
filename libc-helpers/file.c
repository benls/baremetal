
#include "uart.h"

#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

ssize_t _write(int fd, const void *buf, size_t count)
{
    if(fd == 1 || fd == 2) {
        for(unsigned i = 0; i < count; i++)
            uart_putc(((const char*)buf)[i]);
        return count;
    }
    return -1;
}

ssize_t _read(int fd, void *buf, size_t count)
{
    (void)fd;
    (void)buf;
    (void)count;
    return -1;
}

int _isatty(int fd)
{
    return fd < 3;
}

int _close(int fd)
{
    (void)fd;
    return 0;
}

off_t _lseek(int fildes, off_t offset, int whence)
{
    (void)fildes;
    (void)offset;
    (void)whence;
    return -1;
}

int _fstat(int fd, struct stat *buf)
{
    if (fd < 3) {
        memset(buf, 0, sizeof(*buf));
        buf->st_mode = S_IFCHR | 0777;
        return 0;
    }
    return -1;
}

