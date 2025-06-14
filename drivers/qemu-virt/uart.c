#include "uart.h"
#include "mmio.h"
#include <stdint.h>

void uart_init ()
{
    uint8_t *const ptr = (uint8_t *)UART0_BASE;
    uint8_t lcr = (1 << 0) | (1 << 1);
    REG8 (ptr + 3, 0) = lcr;
    REG8 (ptr + 2, 0) = 1<<0;
    REG8 (ptr + 1, 0) = 1<<0;

    uint16_t divisor = 592;
    uint8_t divisor_least = divisor & 0xff;
    uint8_t divisor_most = divisor >> 8;
    REG8 (ptr + 3, 0) = lcr | (1 << 7);

    REG8 (ptr + 0, 0) = divisor_least;
    REG8 (ptr + 1, 0) = divisor_most;

    REG8 (ptr + 3, 0) = lcr;
}

void uart_putc (char c)
{
    while (!UART0_FF_THR_EMPTY);
    UART0_DR = c;
}

void uart_puts (const char *str)
{
    while (*str)
        uart_putc(*str++);
}

int uart_getc ()
{
    while (!(UART0_LSR & UARTLSR_DR));
    return UART0_DR;
}

