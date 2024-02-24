#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <sys/mman.h>

struct mem_block
{
    struct mem_block *next;
    struct mem_block *prev;
    size_t size;
    int size1;
    char ptr[];
};

struct page
{
    struct page *next;
    struct page *prev;
    size_t size;
    struct mem_block *blocks;
};

void *shift(void *blocks, long shift);
void *page_begin(void *ptr, size_t page_size);
void *my_get_page(size_t page_size);
size_t align(size_t size);
size_t my_get_page_size(size_t size);
size_t total_blocks_size(struct mem_block *blocks);
int check_page(struct page *current_page);
void print_page(struct page *current_page);
struct page *mumum(struct page *page_acc, struct page *current_page);

#endif /* !UTILS_H */
