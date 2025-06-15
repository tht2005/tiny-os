#include "syscall.h"
#include "cpu.h"
#include "printf.h"
#include <stdint.h>
#include <inttypes.h>

uintptr_t do_syscall (uintptr_t mepc, TrapFrame *frame)
{
    uintptr_t a0 = frame->regs[10];
    switch (a0)
    {
        case 0:
            return mepc + 4;
        case 1:
            printf ("Test syscall\n");
            return mepc + 4;
        default:
            printf ("Unknown syscall number %"PRIuPTR"\n", a0);
            return mepc + 4;
    }
}

