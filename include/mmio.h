#ifndef __MMIO_H
#define __MMIO_H

#include <stdint.h>

#define REG8(base, offset)                      ((*((volatile uint8_t*)(base + offset))))
#define REG32(base, offset)                     ((*((volatile uint32_t*)(base + offset))))
#define REG64(base, offset)                     ((*((volatile uint64_t*)(base + offset))))

#endif
