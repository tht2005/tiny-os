#include "malloc.h"
#include "kmem.h"
#include <stddef.h>

void *__malloc(size_t size)
{
    return (void*) kmalloc (size);
}
void *__calloc(size_t num, size_t size)
{
    return (void*) kzalloc (num * size);
}
void __free(void *ptr)
{
    kfree (ptr);
}
