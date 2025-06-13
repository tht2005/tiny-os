#include "page.h"
#include "printf.h"
#include "linker_symbol.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

static uintptr_t ALLOC_START = 0;

int page_is_free (const Page *page)
{
    return page->flags == EMPTY;
}

int page_is_taken (const Page *page)
{
    return page->flags & TAKEN;
}

int page_is_last (const Page *page)
{
    return page->flags & LAST;
}

void page_clear (Page *page)
{
    page->flags = EMPTY;
}

uintptr_t align_val (uintptr_t val, uintptr_t order)
{
    uintptr_t o = ( (uintptr_t)1 << order ) - 1;
    return (val + o) & (~o);
}

void page_init ()
{
    size_t i, num_pages = HEAP_SIZE / PAGE_SIZE;
    Page *ptr = (Page *) HEAP_START;
    for (i = 0; i < num_pages; ++i)
    {
        ptr[i].flags = EMPTY;
    }
    ALLOC_START = align_val ( HEAP_START + num_pages * sizeof (Page), PAGE_ORDER );
}

// simple greedy allocation algorithm
uint8_t* alloc (size_t pages)
{
    assert (pages > 0);

    size_t num_pages = HEAP_SIZE / PAGE_SIZE;
    Page *ptr = (Page *) HEAP_START;
    int found;
    size_t i, j, k;

    for (i = 0; i < num_pages - pages; ++i)
    {
        found = 0;
        if (page_is_free (&ptr[i]))
        {
            found = 1;
            for (j = i; j < i + pages; ++j) {
                if (page_is_taken (&ptr[j]))
                {
                    found = 0;
                    break;
                }
            }
        }
        if (found)
        {
            for (k = i; k + 1 < i + pages; ++k)
            {
                ptr[k].flags = TAKEN;
            }
            ptr[k].flags = TAKEN | LAST;
            return (uint8_t *)(ALLOC_START + PAGE_SIZE * i);
        }
    }
    return NULL;
}

void dealloc (const uint8_t *ptr)
{
    assert (ptr);
    uintptr_t addr = HEAP_START + ((uintptr_t)ptr - ALLOC_START) / PAGE_SIZE;
    assert (addr >= HEAP_START && addr < HEAP_START + HEAP_SIZE);
    Page *p = (Page *) addr;
    for (; page_is_taken (p) && !page_is_last (p); ++p)
    {
        page_clear (p);
    }
    // check double-free
    assert ( page_is_last (p) );
    page_clear (p);
}

uint8_t *zalloc (size_t pages)
{
    uint8_t *ret = alloc (pages);
    if (ret == NULL)
    {
        return NULL;
    }
    size_t i, size = (PAGE_SIZE * pages) / 8;
    uint64_t *big_ptr = (uint64_t *) ret;
    for (i = 0; i < size; ++i)
    {
        big_ptr[i] = 0;
    }
    return ret;
}

void print_page_allocations ()
{
    size_t num_pages = HEAP_SIZE / PAGE_SIZE;
    const Page *beg = (const Page *) HEAP_START;
    const Page *end = beg + num_pages;
    uintptr_t alloc_beg = ALLOC_START;
    uintptr_t alloc_end = ALLOC_START + num_pages * PAGE_SIZE;

    printf ("\n");
    printf ("PAGE ALLOCATION TABLE\n");
    printf ("META: 0x%p -> 0x%p\n", beg, end);
    printf ("PHYS: 0x%p -> 0x%p\n", alloc_beg, alloc_end);
    printf ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    size_t num = 0;
    while (beg < end)
    {
        if (page_is_taken (beg)) {
            uintptr_t start = (uintptr_t)beg;
            size_t memaddr = ALLOC_START + (start - HEAP_START) * PAGE_SIZE;
            printf ("0x%p => ", memaddr);
            while (1)
            {
                ++num;
                if (page_is_last (beg))
                {
                    uintptr_t end = (uintptr_t)beg;
                    size_t memaddr = ALLOC_START + (end - HEAP_START) * PAGE_SIZE + PAGE_SIZE - 1;
                    printf ("0x%p: %3d page(s).\n", memaddr, (end - start + 1));
                    break;
                }
                ++beg;
            }
        }
        ++beg;
    }
    printf ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf ("Allocated: %6d pages (%10d bytes).\n", num, num * PAGE_SIZE);
    printf ("Free     : %6d pages (%10d bytes).\n", num_pages - num, (num_pages - num) * PAGE_SIZE);
    printf ("\n");
}

int entry_is_valid (const Entry *self)
{
    return self->entry & ENTRY_VALID;
}
int entry_is_invalid (const Entry *self)
{
    return ! entry_is_valid (self);
}
int entry_is_leaf (const Entry *self)
{
    return self->entry & 0xe;
}
int entry_is_branch (const Entry *self)
{
    return ! entry_is_leaf (self);
}
void entry_set_entry (Entry *self, uint32_t entry)
{
    self->entry = entry;
}
uintptr_t entry_get_entry (Entry *self)
{
    return self->entry;
}

void map (Table *root, uintptr_t vaddr, uintptr_t paddr, uint64_t bits, int level)
{
    assert (bits & 0xe);
    uintptr_t vpn[] = { (vaddr >> 12) & 0x1ff, (vaddr >> 21) & 0x1ff, (vaddr >> 30) & 0x1ff };
    uintptr_t ppn[] = { (paddr >> 12) & 0x1ff, (paddr >> 21) & 0x1ff, (paddr >> 30) & 0x3ffffff };
    Entry *v = &root->entries[vpn[2]];
    for (int i = 1; i >= level; --i)
    {
        if (entry_is_invalid (v))
        {
            uint8_t *page = alloc (1);
            entry_set_entry (v, (((uint64_t)page) >> 2) | ENTRY_VALID);
        }
        Entry *entry = (Entry *) ((entry_get_entry (v) & ~0x3ff) << 2);
        v = &entry[vpn[i]];
    }
    uint64_t entry = ((uint64_t)ppn[2] << 28)
                    | ((uint64_t)ppn[1] << 19)
                    | ((uint64_t)ppn[0] << 10)
                    | bits
                    | ENTRY_VALID;
    entry_set_entry (v, entry);
}

void unmap (Table *root)
{
    for (unsigned lv2 = 0; lv2 < TABLE_LEN; ++lv2)
    {
        Entry *entry_lv2 = &root->entries[lv2];
        if (entry_is_valid (entry_lv2) && entry_is_branch (entry_lv2))
        {
            uintptr_t memaddr_lv1 = (entry_get_entry (entry_lv2) & ~0x3ff) << 2;
            Table *table_lv1 = (Table *)memaddr_lv1;
            for (unsigned lv1 = 0; lv1 < TABLE_LEN; ++lv1)
            {
                Entry *entry_lv1 = &table_lv1->entries[lv1];
                if (entry_is_valid (entry_lv1) && entry_is_branch (entry_lv1))
                {
                    uintptr_t memaddr_lv0 = (entry_get_entry (entry_lv1) & ~0x3ff) << 2;
                    dealloc ((uint8_t *)memaddr_lv0);
                }
            }
            dealloc ((uint8_t *)memaddr_lv1);
        }
    }
}

uintptr_t virt_to_phys (Table *root, uintptr_t vaddr)
{
    uintptr_t vpn[] = { (vaddr >> 12) & 0x1ff, (vaddr >> 21) & 0x1ff, (vaddr >> 30) & 0x1ff };
    Entry *v = &root->entries[vpn[2]];
    for (int i = 2; i >= 0; --i)
    {
        printf ("virt_to_phys debug: 0x%p\n", entry_get_entry (v));
        if (entry_is_invalid (v))
        {
            // invalid entry, page fault
            break;
        }
        else if (entry_is_leaf (v))
        {
            uintptr_t off_mask = (1ULL << (12 + i * 9)) - 1;
            uintptr_t vaddr_pgoff = vaddr & off_mask;
            uintptr_t addr = ((uintptr_t)(entry_get_entry (v) << 2)) & ~off_mask;
            return vaddr_pgoff | addr;
        }
        Entry *entry = (Entry *) ((entry_get_entry (v) & ~0x3ff) << 2);
        assert (i > 0);
        v = &entry[vpn[i - 1]];
    }
    return INVALID_PHYS_ADDR;
}

