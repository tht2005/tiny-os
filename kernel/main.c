#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <inttypes.h>

#include "clint.h"
#include "console.h"
#include "cpu.h"
#include "plic.h"
#include "process.h"
#include "sched.h"
#include "kmem.h"
#include "page.h"
#include "printf.h"
#include "lib.h"
#include "symbol/asm_symbol.h"

void abort ()
{
    __asm__ volatile ("wfi");
}

void panic (const char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);

    printf ("Kernel panic: ");
    vprintf (fmt, args);
    printf ("\n");

    va_end (args);

    abort ();
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
    uintptr_t ret = global_process_init ();
    printf ("Init process created at address 0x%"PRIxPTR"\n", ret);
    plic_set_threshold (0);
    plic_enable (10);
    plic_set_priority (10, 1);
    printf ("UART interupts have been enabled.\n");
    CLINT_SET_TIMEOUT(0, (int)1e6);
    ScheduleContext sContext = schedule ();
    switch_to_user (sContext.frame_addr, sContext.mepc, sContext.satp);
    panic ("switch_to_user return!?");
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

