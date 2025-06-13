#ifndef __PAGE_H
#define __PAGE_H

#define PAGE_SIZE (1 << 12)
#define PAGE_ORDER 12

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {

    EMPTY = 0,
    TAKEN = 1 << 0,
    LAST = 1 << 1,

} PageBits;

typedef struct {
    int8_t flags;
} Page;

uintptr_t align_val (uintptr_t val, uintptr_t order);

void page_init ();
uint8_t* alloc (size_t pages);
void dealloc(const uint8_t *ptr);
uint8_t *zalloc (size_t pages);
void print_page_allocations ();

typedef enum {

    ENTRY_NONE = 0,
    ENTRY_VALID = 1 << 0,
    ENTRY_READ = 1 << 1,
    ENTRY_WRITE = 1 << 2,
    ENTRY_EXECUTE = 1 << 3,
    ENTRY_USER = 1 << 4,
    ENTRY_GLOBAL = 1 << 5,
    ENTRY_ACCESS = 1 << 6,
    ENTRY_DIRTY = 1 << 7,

    ENTRY_READ_WRITE = ENTRY_READ | ENTRY_WRITE,
    ENTRY_READ_EXECUTE = ENTRY_READ | ENTRY_EXECUTE,
    ENTRY_READ_WRITE_EXECUTE = ENTRY_READ | ENTRY_WRITE | ENTRY_EXECUTE,

    ENTRY_USER_READ_WRITE = ENTRY_READ | ENTRY_WRITE | ENTRY_USER,
    ENTRY_USER_READ_EXECUTE = ENTRY_READ | ENTRY_EXECUTE | ENTRY_USER,
    ENTRY_USER_READ_WRITE_EXECUTE = ENTRY_READ | ENTRY_WRITE | ENTRY_EXECUTE | ENTRY_USER,

} EntryBits;

typedef struct {
    uintptr_t entry;
} Entry;

#define TABLE_LEN 512
typedef struct {
    Entry entries[TABLE_LEN];
} Table;

#define INVALID_PHYS_ADDR       (~(uintptr_t)0)

void map (Table *root, uintptr_t vaddr, uintptr_t paddr, uint64_t bits, int level);
uintptr_t virt_to_phys (Table *root, uintptr_t vaddr);

#endif
