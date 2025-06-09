#ifndef __LINKER_SYMBOL_H
#define __LINKER_SYMBOL_H

extern char _text_start[];
extern char _text_end[];
extern char _rodata_start[];
extern char _rodata_end[];
extern char _data_start[];
extern char _data_end[];
extern char _bss_start[];
extern char _bss_end[];
extern char _heap_start[];
extern char _heap_size[];
extern char _stack_start[];
extern char _stack_end[];

#define TEXT_START ((uintptr_t)_text_start)
#define TEXT_END ((uintptr_t)_text_end)
#define DATA_START ((uintptr_t)_data_start)
#define DATA_END ((uintptr_t)_data_end)
#define RODATA_START ((uintptr_t)_rodata_start)
#define RODATA_END ((uintptr_t)_rodata_end)
#define BSS_START ((uintptr_t)_bss_start)
#define BSS_END ((uintptr_t)_bss_end)
#define HEAP_START ((uintptr_t)_heap_start)
#define HEAP_SIZE ((uintptr_t)_heap_size)
#define KERNEL_STACK_START ((uintptr_t)_stack_start)
#define KERNEL_STACK_END ((uintptr_t)_stack_end)

#endif
