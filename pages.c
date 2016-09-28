#include "os.h"
#include "printf.h"
#include "pages.h"

#define MEM_BASE 0x80000000
#define MEM_SZ (512 << 20)
#define PAGE_SHIFT 12
#define PAGE_SZ (1 << PAGE_SHIFT)
#define N_PAGES (MEM_SZ/PAGE_SZ)
#define ADDR_TO_PAGE(addr) (((ptrdiff_t)(addr) - MEM_BASE ) >> PAGE_SHIFT)
#define PAGE_TO_ADDR(page) ((void*)(((ptrdiff_t)(page) << PAGE_SHIFT) + MEM_BASE))

extern char __start;
extern char __end;

static u32 pages[(N_PAGES + 31)/32];
static uint cursor;
static uint free_pages;

static inline void set_page(uint page) {
    free_pages--;
    pages[page/32] |= 1 << (page % 32);
}

static uint get_page_bit(uint page) {
    return !!(pages[page/32] & (1 << (page%32)));
}

void init_pages(void) {
    uint start_page;
    uint end_page;
    uint i;

    free_pages = N_PAGES;

    start_page = ADDR_TO_PAGE(&__start);
    end_page = ADDR_TO_PAGE(&__end);

    /* mark all pages currently in use */
    for (i = start_page; i < end_page; i++)
        set_page(i);

    printf("%u free pages\n", free_pages);
}

//TODO: need contiguous pages?
void* alloc_page(void) {
    uint page;
    if (free_pages == 0)
        return NULL;
    for (;;) {
        page = cursor;
        cursor = (cursor + 1) % N_PAGES;
        if (!get_page_bit(page)) {
            set_page(page);
            return PAGE_TO_ADDR(page);
        }
    }
}

