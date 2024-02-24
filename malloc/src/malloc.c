#include "malloc.h"

#include <stddef.h>
#include <string.h>
#include <sys/mman.h>

#include "utils.h"

static struct page *page_list = NULL;

void *new_page_block(void *new_page, size_t size_new_page, size_t aligned_size)
{
    if (!new_page)
        return NULL;

    struct page *new_page_entry = new_page;
    new_page_entry->size = size_new_page;

    new_page_entry->blocks = (struct mem_block *)(new_page_entry + 1);

    new_page_entry->prev = NULL;
    if (page_list)
        page_list->prev = new_page_entry;
    new_page_entry->next = page_list;
    page_list = new_page_entry;

    struct mem_block *new_block = new_page_entry->blocks;
    new_block->size = aligned_size;
    new_block->next = NULL;
    new_block->prev = NULL;

    return new_block + 1;
}

#ifdef DEBUG
__attribute__((visibility("default"))) void *my_malloc(size_t size)
#else /*  DEBUG */
__attribute__((visibility("default"))) void *malloc(size_t size)
#endif /* DEBUG */
{
    size_t aligned_size = align(size);

    struct page *current_page = page_list;
    while (current_page)
    {
        size_t total = total_blocks_size(current_page->blocks);
        if (current_page->size
            >= total + aligned_size + sizeof(struct mem_block))
        {
            struct mem_block *temp = current_page->blocks;
            struct mem_block *new_block = shift(temp + 1, temp->size);

            new_block->size = aligned_size;
            current_page->blocks->prev = new_block;
            new_block->next = current_page->blocks;
            new_block->prev = NULL;
            current_page->blocks = new_block;
            return new_block + 1;
        }
        current_page = current_page->next;
    }

    void *new_page = my_get_page(aligned_size + sizeof(struct mem_block));
    if (!new_page)
        return NULL;
    size_t size_new_page =
        my_get_page_size(aligned_size + sizeof(struct mem_block));
    return new_page_block(new_page, size_new_page, aligned_size);
}

#ifdef DEBUG
__attribute__((visibility("default"))) void my_free(void *ptr)
#else /* DEBUG */
__attribute__((visibility("default"))) void free(void *ptr)
#endif /* DEBUG */
{
    if (!ptr)
        return;
    struct page *current_page = page_list;
    struct page *page_acc = current_page;
    while (page_acc)
    {
        struct mem_block *new_block = page_acc->blocks;
        struct mem_block *temp_block = new_block;
        while (temp_block)
        {
            if (temp_block + 1 == ptr)
            {
                if (!temp_block->prev && !temp_block->next)
                {
                    new_block = NULL;
                }
                else if (!temp_block->prev)
                {
                    new_block = new_block->next;
                    new_block->prev = NULL;
                }
                else if (!temp_block->next)
                {
                    temp_block->prev->next = NULL;
                }
                else
                {
                    temp_block->prev->next = temp_block->next;
                    temp_block->next->prev = temp_block->prev;
                }

                page_acc->blocks = new_block;

                if (!page_acc->blocks)
                    page_list = mumum(page_acc, current_page);
                else
                    page_list = current_page;

                return;
            }
            temp_block = temp_block->next;
        }
        page_acc = page_acc->next;
    }
}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size)
{
    if (ptr && size == 0)
    {
        free(ptr);
        return NULL;
    }

    void *res = malloc(size);
    if (!res)
        return NULL;

    if (!ptr)
        return res;

    struct mem_block *block = shift(ptr, -sizeof(struct mem_block));
    size_t mini = (size < block->size) ? size : block->size;
    memmove(res, ptr, mini);
    free(ptr);
    return res;
}

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    size_t offset;
    if (__builtin_mul_overflow(nmemb, size, &offset))
        return NULL;

    void *res = malloc(offset);
    if (!res)
        return NULL;

    memset(res, 0, offset);

    return res;
}
