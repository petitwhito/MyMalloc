#include "allocator.h"

#include <stddef.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "utils.h"

struct blk_allocator *blka_new(void)
{
    struct blk_allocator *res = malloc(sizeof(struct blk_allocator));
    res->meta = NULL;
    return res;
}

struct blk_meta *blka_alloc(struct blk_allocator *blka, size_t size)
{
    size_t page_size = sysconf(_SC_PAGESIZE);
    size_t new_size = page_size;
    while (size >= new_size)
        new_size += page_size;

    void *addr = mmap(NULL, new_size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    struct blk_meta *res = addr;
    res->size = new_size - sizeof(struct blk_meta);
    res->next = blka->meta;
    blka->meta = res;

    return res;
}

void blka_free(struct blk_meta *blk)
{
    if (blk)
        munmap(blk, blk->size + sizeof(struct blk_meta));
}

void blka_pop(struct blk_allocator *blka)
{
    if (!blka || !blka->meta)
        return;

    struct blk_meta *popped_meta = blka->meta;
    blka->meta = blka->meta->next;
    blka_free(popped_meta);
}

void blka_delete(struct blk_allocator *blka)
{
    if (!blka)
        return;

    while (blka->meta)
    {
        struct blk_meta *current = blka->meta;
        blka->meta = blka->meta->next;
        blka_free(current);
    }
    free(blka);
}
