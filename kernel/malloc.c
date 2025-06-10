#include "malloc.h"
#include "kmem.h"
#include <stddef.h>

void *malloc(size_t size)
{
    return (void*) kmalloc (size);
}
void *calloc(size_t num, size_t size)
{
    return (void*) kzalloc (num * size);
}
void free(void *ptr)
{
    dealloc (ptr);
}
