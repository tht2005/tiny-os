#include "uart.h"

#define UART0_BASE 0x10000000

#define REG(base, offset)           ((*((volatile unsigned char *)(base + offset))))
#define UART0_DR                    REG(UART0_BASE, 0x00)
#define UART0_FCR                   REG(UART0_BASE, 0x02)
#define UART0_LSR                   REG(UART0_BASE, 0x05)

#define UARTFCR_FFENA               0x01
#define UARTLSR_THRE                0x20
#define UARTLSR_DR                  0x01

#define UART0_FF_THR_EMPTY           (UART0_LSR & UARTLSR_THRE)

void uart_init ()
{
    UART0_FCR = UARTFCR_FFENA;
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

