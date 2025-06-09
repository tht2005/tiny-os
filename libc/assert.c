#include "printf.h"

void __assert_func (const char *assertion, const char *file,
        unsigned int line, const char *func)
{
    printf ("Assert failed at file %s func %s line %u: `%s`\n", file, func, line, assertion);
    while (1) {}
}
