#include "process.h"
#include "cpu.h"
#include "lib.h"
#include "page.h"
#include "malloc.h"
#include "libc/string.h"
#include "printf.h"
#include "symbol/asm_symbol.h"

#include <stdint.h>
#include <inttypes.h>

static Process *FIRST_PROCESS = NULL;
static Process *LAST_PROCESS = NULL;

Process *get_first_process ()
{
    return FIRST_PROCESS;
}
Process *get_last_process ()
{
    return LAST_PROCESS;
}
void process_push_front (Process *proc)
{
    if (FIRST_PROCESS)
    {
        proc->next = FIRST_PROCESS;
        FIRST_PROCESS = proc;
    }
    else
    {
        FIRST_PROCESS = LAST_PROCESS = proc;
    }
}
Process *process_pop_front ()
{
    Process *proc = FIRST_PROCESS;
    if (proc)
    {
        FIRST_PROCESS = proc->next;
        if (FIRST_PROCESS == NULL)
        {
            LAST_PROCESS = NULL;
        }
    }
    return proc;
}
void process_push_back (Process *proc)
{
    if (LAST_PROCESS)
    {
        LAST_PROCESS->next = proc;
        LAST_PROCESS = proc;
    }
    else {
        FIRST_PROCESS = LAST_PROCESS = proc;
    }
}
void process_rotate_left (int count)
{
    if (FIRST_PROCESS)
    {
        Process *proc;
        while (count -- > 0)
        {
            proc = process_pop_front ();
            process_push_back (proc);
        }
    }
}

static uint16_t PID_COUNTER = 1;

void userspace_process ()
{
    uintptr_t i = 0;
    while (1)
    {
        ++i;
        if (i > (int)7e7)
        {
            make_syscall (1);
            i = 0;
        }
    }
}

void add_process_default (void (*pr)())
{
    Process *proc = new_process (pr);
    process_push_front (proc);
}

uintptr_t global_process_init ()
{
    add_process_default (userspace_process);
    Process *p = get_first_process();
    uintptr_t func_vaddr = p->pc;
    uintptr_t frame_addr = (uintptr_t) p->frame;
    printf ("Init's frame is at 0x%"PRIxPTR"\n", frame_addr);
    return func_vaddr;
}

Process *new_process (void (*func)())
{
    uintptr_t func_addr = (uintptr_t) func;
    uintptr_t func_vaddr = func_addr;

    Process *ret_proc = (Process *) malloc (sizeof (Process));
    if (ret_proc == NULL) {
        panic ("malloc");
    }
    memset (ret_proc, 0, sizeof (Process));
    ret_proc->frame = (TrapFrame *) zalloc (1);
    ret_proc->stack = alloc (CONFIG_PROCESS_STACK_PAGES);
    ret_proc->pc = func_vaddr;
    ret_proc->pid = PID_COUNTER++;
    ret_proc->root = (Table*)zalloc (1),
    ret_proc->state = P_RUNNING;

    // x2 ~ sp
    ret_proc->frame->regs[2] = CONFIG_PROCESS_STACK_ADDR + PAGE_SIZE * CONFIG_PROCESS_STACK_PAGES;

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
        printf ("Map stack memory: 0x%16"PRIxPTR" -> 0x%16"PRIxPTR"\n", CONFIG_PROCESS_STACK_ADDR + addr, saddr + addr);
    }

    for (int i = 0; i <= 100; ++i)
    {
        uintptr_t modifier = i * 0x1000ULL;
        map (
            pt,
            func_vaddr + modifier,
            func_addr + modifier,
            ENTRY_USER_READ_WRITE_EXECUTE,
            0
        );
    }

    // map make_syscall function
    map (pt, 0x80000000, 0x80000000, ENTRY_USER_READ_EXECUTE, 0);

    return ret_proc;
}

void free_process (Process *p)
{
    dealloc ((uint8_t*)p->frame);
    dealloc (p->stack);
    unmap (p->root);
    dealloc ((uint8_t*)p->root);
    free (p);
}

