#ifndef __CLINT_H
#define __CLINT_H

#include "mmio.h"

#define CLINT_BASE                              0x02000000
#define CLINT_CLOCK_FREQ                        10000000UL  // 10^7 Hz

#define CLINT_MSIP(hartid)                      REG32(CLINT_BASE, ((hartid) << 2))

#define MTIMECMP(hartid)                        REG64(CLINT_BASE, 0x4000 + ((hartid) << 3))
#define MTIME                                   REG64(CLINT_BASE, 0xbff8)

#define CLINT_SET_TIMEOUT(hartid, duration)     do { MTIMECMP(hartid) = MTIME + (duration); } while (0)

#endif
