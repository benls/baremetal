#include "os.h"

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define PRINTABLE_CHAR(c) ((c) >= 32 && (c) <= 126)
#define USABLE_CHAR(c) (PRINTABLE_CHAR(c) && (c) != ' ')
//TODO
#define ENTER_CHAR(c) ((c) == '\n' || (c) == '\r' || (c) == 13)

static void do_cmd_with_arg(const char * cmd, const char * arg);
static void do_cmd(const char * cmd);

static void write_s(const char *s)
{
    write(2, s, strlen(s));
}

static void echo_char(char c)
{
    if (c == 127)
        /* del -> backspace, del*/
        write_s("\x8\x7f");
    else if (ENTER_CHAR(c))
        write_s("\r\n");
    else if (PRINTABLE_CHAR(c))
        write(2, &c, 1);
}

static void handle_char(void)
{
    static unsigned bufpos;
    static char buf[100];
    char c;
    read(0, &c, 1);
    if (ENTER_CHAR(c)) {
        echo_char(c);
        buf[bufpos] = 0;
        do_cmd(buf);
        bufpos = 0;
    } else if ((c == 8 || c == 127) && bufpos > 0) {
        echo_char(c);
        bufpos--;
    } else if (PRINTABLE_CHAR(c) && bufpos < COUNT_OF(buf) - 1) {
        echo_char(c);
        buf[bufpos++] = c;
    }
}

static void do_cmd(const char * cmd)
{
static char cmd_name[10];
unsigned i;
/* copy command name to buffer */
i = 0;
while (i < COUNT_OF(cmd_name) - 1 && USABLE_CHAR(*cmd))
    cmd_name[i++] = *(cmd++);
cmd_name[i] = 0;

/* skip over whitespace */
while (*cmd != 0 && !USABLE_CHAR(*cmd))
    cmd++;

do_cmd_with_arg(cmd_name, cmd);
}

static void do_cmd_with_arg(const char * cmd, const char * arg)
{
    static char buf[100];
    char *endptr;
    u32 *addr;
    u32 result;

    endptr = (void*)arg;
    addr = (void*)strtoul(arg, &endptr, 0);
    if (*arg == 0 || *endptr != 0) {
        write_s("Invalid address: ");
        write_s(arg);
        write_s("\r\n");
    }

    if (strcmp(cmd, "r") == 0) {
        result = *addr;
        sprintf(buf, "0x%08lx\r\n", result);
        write_s(buf);
    } else if (strcmp(cmd, "r16") == 0) {
        result = *(u16*)addr;
        sprintf(buf, "0x%04lx\r\n", result);
        write_s(buf);
    } else if (strcmp(cmd, "r8") == 0) {
        result = *(u8*)addr;
        sprintf(buf, "0x%02lx\r\n", result);
        write_s(buf);
    } else {
        write_s("Invalid command: ");
        write_s(cmd);
        write_s("\r\n");
    }
}

void debug_main(void)
{
    write_s("In debug main\r\n");
    for(;;) handle_char();
}

