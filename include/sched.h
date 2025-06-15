#ifndef __SCHED_H
#define __SCHED_H

#include <stdint.h>

typedef struct {
    uintptr_t frame_addr;
    uintptr_t mepc;
    uintptr_t satp;
} ScheduleContext;

static inline ScheduleContext make_schedule_context (uintptr_t frame_addr, uintptr_t mepc, uintptr_t satp) {
    ScheduleContext ctx;
    ctx.frame_addr = frame_addr;
    ctx.mepc = mepc;
    ctx.satp = satp;
    return ctx;
}

ScheduleContext schedule ();

#endif
