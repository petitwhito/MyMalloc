#include "alignment.h"

#include <stddef.h>

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
