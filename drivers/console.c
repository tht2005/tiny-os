#include "console.h"
#include "uart.h"

void console_init ()
{
    uart_init();
}

void console_putc (char c)
{
    uart_putc (c);
}

void console_puts (const char *str)
{
    uart_puts (str);
}

void _putchar (char c)
{
    console_putc (c);
}
