#ifndef __KMEM_h
#define __KMEM_h

#include "page.h"
#include <stdint.h>

typedef enum {
    ALLOC_LIST_TAKEN,
} AllocListFlags;

typedef struct AllocList {
    uintptr_t flags_size;
} AllocList;

uint8_t *get_head ();
Table *get_page_table ();
uintptr_t get_num_allocations ();
void kmem_init ();

uint8_t *kmalloc (uintptr_t sz);
uint8_t *kzalloc (uintptr_t sz);
void kfree (uint8_t *ptr);

void kmem_print_table ();

#endif
