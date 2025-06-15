#ifndef __SYSCALL_H
#define __SYSCALL_H

#include "cpu.h"
#include <stdint.h>

uintptr_t do_syscall (uintptr_t mepc, TrapFrame *frame);

#endif
