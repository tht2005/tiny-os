#ifndef __ASM_SYMBOL_H
#define __ASM_SYMBOL_H

#include <stdint.h>

// trap.S
void switch_to_user (uintptr_t frame_addr, uintptr_t mepc, uintptr_t satp);
uintptr_t make_syscall (uintptr_t a0);

#endif
