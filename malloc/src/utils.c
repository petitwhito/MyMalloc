#include "utils.h"

#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>

#include "malloc.h"

void *page_begin(void *ptr, size_t page_size)
{
    size_t page_mask = ~(page_size - 1);
    size_t page_start = (size_t)ptr;
    unsigned char *n = NULL;
    return n + (page_mask & page_start);
}

void *my_get_page(size_t size)
{
    size_t page_size = sysconf(_SC_PAGESIZE);
    size_t new_size = page_size;
    size += sizeof(struct page);
    while (size > new_size)
        new_size += page_size;

    void *addr = mmap(NULL, new_size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (addr == MAP_FAILED)
        return NULL;

    return addr;
}

size_t my_get_page_size(size_t size)
{
    size_t page_size = sysconf(_SC_PAGESIZE);
    size_t new_size = page_size;
    size += sizeof(struct page);
    while (size > new_size)
        new_size += page_size;

    return new_size;
}

size_t align(size_t size)
{
    size_t mod;
    mod = size % sizeof(long double);

    if (mod)
    {
        mod = sizeof(long double) - mod;
    }

    size_t offset;
    if (__builtin_add_overflow(size, mod, &offset))
        return 0;

    return offset;
}

void *shift(void *blocks, long shift)
{
    unsigned char *res = blocks;
    res += shift;
    return res;
}

size_t total_blocks_size(struct mem_block *blocks)
{
    size_t total = 0;
    while (blocks)
    {
        total += blocks->size + sizeof(struct mem_block);
        blocks = blocks->next;
    }
    return total + sizeof(struct page);
}

int check_page(struct page *current_page)
{
    struct page *temp_page = NULL;
    temp_page = current_page->next;

    if (!current_page->blocks)
    {
        munmap(current_page, current_page->size);
        current_page = temp_page;
        return 1;
    }
    return 0;
}

struct page *mumum(struct page *page_acc, struct page *current_page)
{
    if (!page_acc->prev && !page_acc->next)
        current_page = NULL;
    else if (!page_acc->prev)
    {
        current_page = current_page->next;
        current_page->prev = NULL;
    }
    else if (!page_acc->next)
        page_acc->prev->next = NULL;
    else
    {
        page_acc->prev->next = page_acc->next;
        page_acc->next->prev = page_acc->prev;
    }

    munmap(page_acc, page_acc->size);
    return current_page;
}
