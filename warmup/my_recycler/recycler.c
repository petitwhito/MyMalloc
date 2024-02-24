#include "recycler.h"

#include <stddef.h>
#include <stdlib.h>

struct recycler *recycler_create(size_t block_size, size_t total_size)
{
    if (block_size % sizeof(size_t) != 0 || block_size == 0 || total_size == 0
        || total_size % block_size != 0)
        return NULL;

    struct recycler *res = malloc(sizeof(struct recycler));
    if (!res)
        return NULL;

    res->block_size = block_size;
    res->capacity = total_size / block_size;
    res->chunk = malloc(total_size);
    res->free = NULL;

    if (!res->chunk)
    {
        free(res);
        return NULL;
    }

    struct free_list *block_ptr = res->chunk;
    struct free_list *tmp = block_ptr;

    for (size_t i = 0; i < res->capacity; i++)
    {
        tmp->next = tmp + block_size / sizeof(struct free_list);
        tmp = tmp->next;
    }
    tmp = tmp - block_size / sizeof(struct free_list);
    tmp->next = NULL;

    res->free = block_ptr;
    return res;
}

void recycler_destroy(struct recycler *r)
{
    if (!r)
        return;

    free(r->chunk);
    free(r);
}

void *recycler_allocate(struct recycler *r)
{
    if (!r || !r->free)
        return NULL;

    struct free_list *tmp = r->free;
    r->free = tmp->next;
    return tmp;
}

void recycler_free(struct recycler *r, void *block)
{
    if (!r || !block)
        return;

    struct free_list *tmp = block;
    tmp->next = r->free;
    r->free = tmp;
}
