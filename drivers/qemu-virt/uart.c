#include "uart.h"

void uart_init ()
{
    // UART0_FCR = UARTFCR_FFENA;
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

char uart_getc ()
{
    while (!(UART0_LSR & UARTLSR_DR));
    return UART0_DR;
}

