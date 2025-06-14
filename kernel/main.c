#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <inttypes.h>

#include "clint.h"
#include "console.h"
#include "cpu.h"
#include "uart.h"
#include "kmem.h"
#include "page.h"
#include "printf.h"
#include "linker_symbol.h"
#include "malloc.h"
#include "lib.h"

void panic (const char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);

    printf ("Kernel panic: ");
    vprintf (fmt, args);
    printf ("\n");

    va_end (args);

    while (1)
    {
        __asm__ volatile ("wfi");
    }
}

void kmain()
{
    console_init ();
    {
        uint32_t *k = (uint32_t *) malloc (sizeof (uint32_t));
        *k = 10;
        printf ("uint32_t value = %u\n", *k);

        uint32_t *vec = (uint32_t *) malloc (4 * sizeof (uint32_t));
        vec[0] = 240;
        vec[1] = 159;
        vec[2] = 146;
        vec[3] = 150;

        for (int i = 0; i < 4; ++i) {
            printf ("vec[%d] = %u\n", i, vec[i]);
        }

        kmem_print_table ();
        free (vec);
        free (k);
    }

    printf ("Every thing is freed now\n");
    kmem_print_table ();

    printf ("debug: %lu %lu\n", MTIMECMP (0), MTIME);
    CLINT_SET_TIMEOUT (0, CLINT_CLOCK_FREQ);

    while (1)
    {
    }
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

void kinit ()
{
    console_init ();
    page_init ();
    kmem_init();
    Table *root = get_page_table();
    uint8_t *kheap_head = get_head();
    uintptr_t total_pages = get_num_allocations();
    printf ("\n");
    printf ("\n");
    printf ("TEXT:   0x%"PRIxPTR" -> 0x%"PRIxPTR"\n", TEXT_START, TEXT_END);
    printf ("RODATA: 0x%"PRIxPTR" -> 0x%"PRIxPTR"\n", RODATA_START, RODATA_END);
    printf ("DATA:   0x%"PRIxPTR" -> 0x%"PRIxPTR"\n", DATA_START, DATA_END);
    printf ("BSS:    0x%"PRIxPTR" -> 0x%"PRIxPTR"\n", BSS_START, BSS_END);
    printf ("STACK:  0x%"PRIxPTR" -> 0x%"PRIxPTR"\n", KERNEL_STACK_START, KERNEL_STACK_END);
    printf ("HEAP:   0x%"PRIxPTR" -> 0x%"PRIxPTR"\n", kheap_head, kheap_head + total_pages * PAGE_SIZE);
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

    id_map_range (root, UART0_BASE, UART0_BASE + 0x100, ENTRY_READ_WRITE);
    id_map_range (root, CLINT_BASE, CLINT_BASE + 0xffff, ENTRY_READ_WRITE);
    // PLIC
    id_map_range (root, 0x0c000000, 0x0c002000, ENTRY_READ_WRITE);
    id_map_range (root, 0x0c200000, 0x0c208000, ENTRY_READ_WRITE);

    uintptr_t satp_value = build_satp (SV39, 0, (uintptr_t) root);
    mscratch_write ((uintptr_t) &KERNEL_TRAP_FRAME[0]);
    sscratch_write ((uintptr_t) &KERNEL_TRAP_FRAME[0]);
    KERNEL_TRAP_FRAME[0].satp = satp_value;
    // stack move from high to low
    KERNEL_TRAP_FRAME[0].trap_stack = (uintptr_t) (zalloc(1) + PAGE_SIZE);

    id_map_range (
        root,
        (uintptr_t)((uint8_t*)KERNEL_TRAP_FRAME[0].trap_stack - PAGE_SIZE),
        KERNEL_TRAP_FRAME[0].trap_stack,
        ENTRY_READ_WRITE
    );
    id_map_range (
        root,
        mscratch_read(),
        mscratch_read() + sizeof (TrapFrame),
        ENTRY_READ_WRITE
    );
    print_page_allocations();
    uintptr_t p = KERNEL_TRAP_FRAME[0].trap_stack - 1;
    uintptr_t m = virt_to_phys (root, p);
    printf ("Walk: 0x%"PRIxPTR" = 0x%"PRIxPTR"\n", p, m);

    printf ("Setting 0x%"PRIxPTR"\n", satp_value);
    printf ("Scratch reg = 0x%"PRIxPTR"\n", mscratch_read());

    satp_write (satp_value);
    satp_fence_asid (0);
}

void kinit_hart (uintptr_t hartid)
{
    uintptr_t addr = (uintptr_t) &KERNEL_TRAP_FRAME[hartid];
    mscratch_write (addr);
    sscratch_write (addr);
    KERNEL_TRAP_FRAME[hartid].hartid = hartid;
    // KERNEL_TRAP_FRAME[hartid].satp = KERNEL_TRAP_FRAME[0].satp;
    // KERNEL_TRAP_FRAME[hartid].trap_stack = (uintptr_t) zalloc (1);
}

