#ifndef __PLIC_H
#define __PLIC_H

#include "mmio.h"
#include <stdbool.h>
#include <stdint.h>

#define PLIC_PRIORITY(id)               REG32(0x0c000000, ((uintptr_t)(id)<<2))
#define PLIC_PENDING                    REG32(0x0c001000, 0)
#define PLIC_INT_ENABLE                 REG32(0x0c002000, 0)
#define PLIC_THRESHOLD                  REG32(0x0c200000, 0)
#define PLIC_CLAIM                      REG32(0x0c200004, 0)

static inline uint32_t plic_next ()
{
    return PLIC_CLAIM;
}
static inline void plic_complete (uint32_t id)
{
    PLIC_CLAIM = id;
}

static inline void plic_set_threshold (uint8_t tsh)
{
    PLIC_THRESHOLD = (uint32_t)(tsh & 7);
}

static inline bool plic_is_pending (uint32_t id)
{
    return PLIC_PENDING >> id & 1;
}

static inline void plic_enable (uint32_t id)
{
    const uint32_t enables = PLIC_INT_ENABLE;
    PLIC_INT_ENABLE = enables | (1 << id);
}

static inline void plic_set_priority (uint32_t id, uint32_t prio)
{
    PLIC_PRIORITY (id) = prio & 7;
}

#endif

