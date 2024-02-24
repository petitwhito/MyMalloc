#include <assert.h>
#include <stddef.h>

void *beware_overflow(void *ptr, size_t nmemb, size_t size)
{
    size_t offset;
    if (__builtin_mul_overflow(nmemb, size, &offset))
        return NULL;

    unsigned char *check = ptr;
    return check + offset;
}
