#ifndef __CPU_H
#define __CPU_H

#include <stdint.h>

typedef enum {
    OFF = 0,
    SV39 = 8,
    SV48 = 9,
} SatpMode;

typedef struct {
    uintptr_t regs[32];
    uintptr_t fregs[32];
    uintptr_t satp;
    uintptr_t trap_stack;
    uintptr_t hartid;
} TrapFrame;

extern TrapFrame KERNEL_TRAP_FRAME[CONFIG_MAX_CPUS];

static inline uintptr_t build_satp (SatpMode mode, uintptr_t asid, uintptr_t addr)
{
    return ((uintptr_t)mode << 60)
        | (asid & 0xffff) << 44
        | ((addr >> 12) & 0xffffffffff);
}

static inline uintptr_t mhartid_read (void)
{
    uintptr_t ret;
    __asm__ volatile("csrr %0, mhartid" : "=r"(ret));
    return ret;
}

static inline uintptr_t mstatus_read (void)
{
    uintptr_t ret;
    __asm__ volatile("csrr %0, mstatus" : "=r"(ret));
    return ret;
}

static inline void stvec_write (uintptr_t val)
{
    __asm__ volatile("csrw stvec, %0" ::"r"(val));
}

static inline uintptr_t stvec_read (void)
{
    uintptr_t ret;
    __asm__ volatile("csrr %0, stvec" : "=r"(ret));
    return ret;
}

static inline void mscratch_write (uintptr_t val)
{
    __asm__ volatile("csrw mscratch, %0" ::"r"(val));
}

static inline uintptr_t mscratch_read (void)
{
    uintptr_t ret;
    __asm__ volatile("csrr %0, mscratch" : "=r"(ret));
    return ret;
}

static inline uintptr_t mscratch_swap (uintptr_t to)
{
    uintptr_t from;
    __asm__ volatile("csrrw %0, mscratch, %1" : "=r"(from) : "r"(to));
    return from;
}

static inline void sscratch_write (uintptr_t val)
{
    __asm__ volatile("csrw sscratch, %0" ::"r"(val));
}

static inline uintptr_t sscratch_read (void)
{
    uintptr_t ret;
    __asm__ volatile("csrr %0, sscratch" : "=r"(ret));
    return ret;
}

static inline uintptr_t sscratch_swap (uintptr_t to)
{
    uintptr_t from;
    __asm__ volatile("csrrw %0, sscratch, %1" : "=r"(from) : "r"(to));
    return from;
}

static inline void sepc_write (uintptr_t val)
{
    __asm__ volatile("csrw sepc, %0" ::"r"(val));
}

static inline uintptr_t sepc_read (void)
{
    uintptr_t ret;
    __asm__ volatile("csrr %0, sepc" : "=r"(ret));
    return ret;
}

static inline void satp_write (uintptr_t val)
{
    __asm__ volatile("csrw satp, %0" ::"r"(val));
}

static inline uintptr_t satp_read (void)
{
    uintptr_t ret;
    __asm__ volatile("csrr %0, satp" : "=r"(ret));
    return ret;
}

static inline void satp_fence (uintptr_t vaddr, uintptr_t asid)
{
    __asm__ volatile("sfence.vma %0, %1" :: "r"(vaddr), "r"(asid));
}

static inline void satp_fence_asid (uintptr_t asid)
{
    __asm__ volatile("sfence.vma zero, %0" :: "r"(asid));
}

static inline void mepc_write (uintptr_t val)
{
    __asm__ volatile("csrw mepc, %0" ::"r"(val));
}

static inline uintptr_t mepc_read (void)
{
    uintptr_t ret;
    __asm__ volatile("csrr %0, mepc" : "=r"(ret));
    return ret;
}

#endif
