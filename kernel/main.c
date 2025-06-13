#include "console.h"
#include "uart.h"
#include "kmem.h"
#include "page.h"
#include "printf.h"
#include "linker_symbol.h"
#include "malloc.h"
#include <assert.h>
#include <stdint.h>

static uintptr_t KERNEL_TABLE;

__attribute__((aligned(32)))
void kmain()
{
    console_init ();
    printf ("#          KMAIN          #\n");
    {
        uint32_t *k = (uint32_t *) malloc (sizeof (uint32_t));
        *k = 10;
        printf ("Value = %u\n", *k);
        kmem_print_table ();
        free (k);
    }
    return ;
}

void id_map_range (Table *root, uintptr_t start, uintptr_t end, uint64_t bits)
{
    uintptr_t memaddr = start & ~(PAGE_SIZE - 1);
    uintptr_t num_kb_pages = (align_val (end, 12) - memaddr) / PAGE_SIZE;
    for (unsigned _  = 0; _ < num_kb_pages; ++_)
    {
        map (root, memaddr, memaddr, bits, 0);
        memaddr += PAGE_SIZE;
    }
}

uintptr_t kinit ()
{
    console_init ();
    page_init ();
    kmem_init();
    Table *root = get_page_table();
    uint8_t *kheap_head = get_head();
    uintptr_t total_pages = get_num_allocations();
    printf ("#          KINIT          #\n");
    printf ("\n");
    printf ("TEXT:   0x%p -> 0x%p\n", TEXT_START, TEXT_END);
    printf ("RODATA: 0x%p -> 0x%p\n", RODATA_START, RODATA_END);
    printf ("DATA:   0x%p -> 0x%p\n", DATA_START, DATA_END);
    printf ("BSS:    0x%p -> 0x%p\n", BSS_START, BSS_END);
    printf ("STACK:  0x%p -> 0x%p\n", KERNEL_STACK_START, KERNEL_STACK_END);
    printf ("HEAP:   0x%p -> 0x%p\n", kheap_head, kheap_head + total_pages * PAGE_SIZE);
    id_map_range (
        root,
        (uintptr_t) kheap_head,
        (uintptr_t) kheap_head + total_pages * PAGE_SIZE,
        ENTRY_READ_WRITE
    );
    uintptr_t num_pages = HEAP_SIZE / PAGE_SIZE;
    id_map_range (
        root,
        HEAP_START,
        HEAP_START + num_pages,
        ENTRY_READ_WRITE
    );
    id_map_range (
        root,
        TEXT_START,
        TEXT_END,
        ENTRY_READ_EXECUTE
    );
    id_map_range (
        root,
        RODATA_START,
        RODATA_END,
        ENTRY_READ_EXECUTE
    );
    id_map_range (
        root,
        DATA_START,
        DATA_END,
        ENTRY_READ_WRITE
    );
    id_map_range (
        root,
        BSS_START,
        BSS_END,
        ENTRY_READ_WRITE
    );
    id_map_range (
        root,
        KERNEL_STACK_START,
        KERNEL_STACK_END,
        ENTRY_READ_WRITE
    );

    map (root, UART0_BASE, UART0_BASE, ENTRY_READ_WRITE, 0);

    map (root, 0x02000000, 0x02000000, ENTRY_READ_WRITE, 0);
    map (root, 0x0200b000, 0x0200b000, ENTRY_READ_WRITE, 0);
    map (root, 0x0200c000, 0x0200c000, ENTRY_READ_WRITE, 0);
    map (root, 0x0c000000, 0x0c002000, ENTRY_READ_WRITE, 0);
    map (root, 0x0c200000, 0x0c208000, ENTRY_READ_WRITE, 0);

    print_page_allocations();

    KERNEL_TABLE = (uintptr_t)root;

    return ((uintptr_t)root >> 12) | (8ULL << 60);
}

