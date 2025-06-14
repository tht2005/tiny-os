#ifndef __UART_H
#define __UART_H

#include "mmio.h"
#include <stdint.h>

#define UART0_BASE 0x10000000

#define UART0_DR                    REG8(UART0_BASE, 0x00)
#define UART0_FCR                   REG8(UART0_BASE, 0x02)
#define UART0_LSR                   REG8(UART0_BASE, 0x05)

#define UARTFCR_FFENA               0x01
#define UARTLSR_THRE                0x20
#define UARTLSR_DR                  0x01

#define UART0_FF_THR_EMPTY           (UART0_LSR & UARTLSR_THRE)

void uart_init ();
void uart_putc (char c);
void uart_puts (const char *str);
int uart_getc ();
#endif
