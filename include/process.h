#ifndef __PROCESS_H
#define __PROCESS_H

#include "cpu.h"
#include "page.h"
#include <stdint.h>

typedef enum {
    P_RUNNING,
    P_SLEEPING,
    P_WAITING,
    P_DEAD,
} ProcessState;

typedef struct {
    uint8_t cwd_path[128];
} ProcessData;

typedef struct Process {
    TrapFrame frame;
    uint8_t* stack;
    uintptr_t pc;
    uint16_t pid;
    Table *root;
    ProcessState state;
    ProcessData data;
    struct Process *next;
} Process;

Process *new_process (void (*func)());
void free_process (Process *p);

#endif
