#include "sched.h"
#include "cpu.h"
#include "process.h"
#include "printf.h"
#include <stdint.h>
#include <inttypes.h>

ScheduleContext schedule ()
{
    if (get_first_process ())
    {
        process_rotate_left (1);
        uintptr_t frame_addr = 0;
        uintptr_t mepc = 0;
        uintptr_t satp = 0;
        uintptr_t pid = 0;
        Process *prc = get_first_process();
        if (prc)
        {
            switch (prc->state)
            { 
                case P_RUNNING:
                    frame_addr = (uintptr_t)prc->frame;
                    mepc = prc->pc;
                    satp = (uintptr_t)prc->root >> 12;
                    pid = prc->pid;
                    break;
                case P_SLEEPING:
                    break;
                default:
                    break;
            }
        }
        printf ("Scheduling %"PRIuPTR"\n", pid);
        if (frame_addr)
        {
            if (satp)
            {
                return make_schedule_context (frame_addr, mepc, ((uintptr_t)SV39 << 60) | (pid << 44) | satp);
            }
            else
            {
                return make_schedule_context (frame_addr, mepc, 0);
            }
        }
    }
    return make_schedule_context (0, 0, 0);
}

