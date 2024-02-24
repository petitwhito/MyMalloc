#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#include <sys/mman.h>

__attribute__((visibility("default"))) void *my_malloc(size_t size);
__attribute__((visibility("default"))) void my_free(void *ptr);

#endif /* !MALLOC_H */
