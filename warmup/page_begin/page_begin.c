#include "page_begin.h"

#include <stddef.h>

void *page_begin(void *ptr, size_t page_size)
{
    size_t page_mask = ~(page_size - 1);
    size_t page_start = (size_t)ptr;
    unsigned char *n = NULL;
    return n + (page_mask & page_start);
}
