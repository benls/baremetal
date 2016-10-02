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
    free_pages--; //TODO: check if already set?
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

void* alloc_pages_contig(uint n_pages) {
    uint start;
    uint prev_cursor;
    uint page;
    uint n;
    if (free_pages < n_pages)
        return NULL;
    start = cursor;
    for (;;) {
        page = cursor;
        n = n_pages;
        do {
            prev_cursor = cursor;
            cursor = (cursor + 1) % N_PAGES;
            if (prev_cursor < page || get_page_bit(prev_cursor))
                break;
            if (cursor == start)
                return NULL;
            } while(--n);
        if (n == 0)
            break;
    }
    for(uint i = 0; i < n_pages; i++) 
        set_page(page + i);
    return PAGE_TO_ADDR(page);
}

