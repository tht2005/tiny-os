#include "libc/string.h"

#include <stdint.h>

void *memset (void *s, int c, size_t n)
{
    uint8_t *p = (uint8_t *) s;
    while (n--) {
        *p++ = (uint8_t)c;
    }
    return s;
}

void *memcpy (void *dest, const void *src, size_t n)
{
    uint8_t *d = (uint8_t *) dest;
    const uint8_t *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

