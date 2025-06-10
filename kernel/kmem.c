#include "kmem.h"
#include "page.h"
#include "printf.h"
#include <stddef.h>
#include <stdint.h>

static AllocList *KMEM_HEAD = NULL;
static uintptr_t KMEM_ALLOC = 0;
static Table *KMEM_PAGE_TABLE = NULL;

int alloc_list_is_taken (AllocList *self)
{
    return self->flags_size & ALLOC_LIST_TAKEN;
}
int alloc_list_is_free (AllocList *self)
{
    return ! alloc_list_is_taken (self);
}
void alloc_list_set_taken (AllocList *self)
{
    self->flags_size |= ALLOC_LIST_TAKEN;
}
void alloc_list_set_free (AllocList *self)
{
    self->flags_size &= ~ALLOC_LIST_TAKEN;
}
void alloc_list_set_size (AllocList *self, uintptr_t sz)
{
    int k = alloc_list_is_taken (self);
    self->flags_size = sz & ~ALLOC_LIST_TAKEN;
    if (k)
    {
        self->flags_size |= ALLOC_LIST_TAKEN;
    }
}
uintptr_t alloc_list_get_size (AllocList *self)
{
    return self->flags_size & ~ALLOC_LIST_TAKEN;
}

uint8_t *get_head ()
{
    return (uint8_t *) KMEM_HEAD;
}
Table *get_page_table ()
{
    return KMEM_PAGE_TABLE;
}
uintptr_t get_num_allocations ()
{
    return KMEM_ALLOC;
}

void kmem_init ()
{
    uint8_t *k_alloc = zalloc (64);
    assert (k_alloc);
    KMEM_ALLOC = 64;
    KMEM_HEAD = (AllocList *) k_alloc;
    alloc_list_set_free (KMEM_HEAD);
    alloc_list_set_size (KMEM_HEAD, KMEM_ALLOC * PAGE_SIZE);
    KMEM_PAGE_TABLE = (Table *) zalloc (1);
}

uint8_t *kmalloc (uintptr_t sz)
{
    uintptr_t size = align_val (sz, 3) + sizeof (AllocList);
    AllocList *head = KMEM_HEAD;
    AllocList *tail = (AllocList *)( ((uint8_t *) KMEM_HEAD) + (KMEM_ALLOC * PAGE_SIZE) );
    uintptr_t chunk_size, rem;
    while (head < tail)
    {
        if (alloc_list_is_free (head) && size <= alloc_list_get_size (head))
        {
            chunk_size = alloc_list_get_size (head);
            rem = chunk_size - size;
            alloc_list_set_taken (head);
            if (rem > sizeof (AllocList))
            {
                AllocList *next = (AllocList *)(((uint8_t *) head) + size);
                alloc_list_set_free (next);
                alloc_list_set_size (next, rem);
                alloc_list_set_size (head, size);
            }
            else
            {
                alloc_list_set_size (head, chunk_size);
            }
            return (uint8_t *) (head + 1);
        }
        else
        {
            head = (AllocList *)( ((uint8_t *) head) + alloc_list_get_size (head) );
        }
    }
    return NULL;
}

uint8_t *kzalloc (uintptr_t sz)
{
    uintptr_t size = align_val (sz, 3);
    uint8_t *ret = kmalloc (size);
    if (ret)
    {
        for (uintptr_t i = 0; i < size; ++i)
        {
            ret[i] = 0;
        }
    }
    return ret;
}

void coalesce ()
{
    AllocList *head = KMEM_HEAD;
    AllocList *tail = (AllocList *)( ((uint8_t *) KMEM_HEAD) + (KMEM_ALLOC * PAGE_SIZE) );
    while (head < tail)
    {
        AllocList *next = (AllocList *) ((uint8_t *)head + alloc_list_get_size (head));
        if (alloc_list_get_size (head) == 0)
        {
            break;
        }
        else if (next >= tail)
        {
            break;
        }
        else if (alloc_list_is_free (head) && alloc_list_is_free (next))
        {
            alloc_list_set_size (head,
                                alloc_list_get_size (head) + alloc_list_get_size (next));
        }
        head = (AllocList *) ((uint8_t *)head + alloc_list_get_size (head));
    }
}

void kfree (uint8_t *ptr)
{
    if (ptr)
    {
        AllocList *p = (AllocList *)ptr - 1;
        if (alloc_list_is_taken (p)) {
            alloc_list_set_free (p);
        }
        coalesce ();
    }
}

void kmem_print_table ()
{
    AllocList *head = KMEM_HEAD;
    AllocList *tail = (AllocList *)( ((uint8_t *) KMEM_HEAD) + (KMEM_ALLOC * PAGE_SIZE) );
    while (head < tail)
    {
        printf ("0x%p: Length = %10d, Taken = %d\n", head, alloc_list_get_size (head), alloc_list_is_taken (head));
        head = (AllocList *) ( (uint8_t *)head + alloc_list_get_size (head) );
    }
}

