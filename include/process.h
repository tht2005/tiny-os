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
    TrapFrame *frame;
    uint8_t* stack;
    uintptr_t pc;
    uint16_t pid;
    Table *root;
    ProcessState state;
    ProcessData data;
    struct Process *next;
    uint64_t sleep_until;
} Process;

Process *get_first_process ();
Process *get_last_process ();
void process_push_front (Process *proc);
// Process *process_pop_front ();
void process_push_back (Process *proc);
void process_rotate_left (int count);

Process *new_process (void (*func)());
void free_process (Process *p);

uintptr_t global_process_init ();

#endif
