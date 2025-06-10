#ifndef __MALLOC_H
#define __MALLOC_H
#include <stddef.h>
void *__malloc(size_t size);
void *__calloc(size_t num, size_t size);
void __free(void *ptr);
#define malloc __malloc
#define calloc __calloc
#define free __free
#endif
