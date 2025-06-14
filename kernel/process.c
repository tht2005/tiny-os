#include "process.h"
#include "cpu.h"
#include "lib.h"
#include "page.h"
#include "malloc.h"
#include "libc/string.h"
#include <stdint.h>

static Process *FIRST_PROCESS = NULL;
static uint16_t PID_COUNTER = 1;

void init_process ()
{

}

void add_process_default (void (*pr)())
{
    Process *proc = new_process (pr);
    proc->next = FIRST_PROCESS;
    FIRST_PROCESS = proc;
}

uintptr_t process_init ()
{
    add_process_default (init_process);
    Process *p = FIRST_PROCESS;
    TrapFrame *frame = &p->frame;
    mscratch_write ((uintptr_t)frame);
    satp_write (build_satp (SV39, 1, (uintptr_t)p->root));
    satp_fence_asid (1);
    return CONFIG_PROCESS_START_ADDR;
}

Process *new_process (void (*func)())
{
    Process *ret_proc = (Process *) malloc (sizeof (Process));
    if (ret_proc == NULL) {
        panic ("malloc");
    }
    memset (ret_proc, 0, sizeof (Process));
    // x2 ~ sp
    ret_proc->frame.regs[2] = CONFIG_PROCESS_STACK_ADDR + PAGE_SIZE * CONFIG_PROCESS_STACK_PAGES;
    ret_proc->stack = alloc (CONFIG_PROCESS_STACK_PAGES);
    ret_proc->pid = PID_COUNTER++;
    ret_proc->root = (Table*)zalloc (1),
    ret_proc->state = P_WAITING;

    Table *pt = ret_proc->root;
    uintptr_t saddr = (uintptr_t)ret_proc->stack;
    for (int i = 0; i < CONFIG_PROCESS_STACK_PAGES; ++i)
    {
        uintptr_t addr = PAGE_SIZE * i;
        map (
            pt,
            CONFIG_PROCESS_STACK_ADDR + addr,
            saddr + addr,
            ENTRY_USER_READ_WRITE,
            0
        );
    }

    uintptr_t func_addr = (uintptr_t)func;
    map (
        pt,
        CONFIG_PROCESS_START_ADDR,
        func_addr,
        ENTRY_USER_READ_EXECUTE,
        0
    );
    map (
        pt,
        CONFIG_PROCESS_START_ADDR + 0x1001,
        func_addr + 0x1001,
        ENTRY_USER_READ_EXECUTE,
        0
    );

    return ret_proc;
}

void free_process (Process *p)
{
    dealloc (p->stack);
    unmap (p->root);
    dealloc ((uint8_t*)p->root);
    free (p);
}

