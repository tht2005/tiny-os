#ifndef __UART_H
#define __UART_H
void uart_init ();
void uart_putc (char c);
void uart_puts (const char *str);
char uart_getc ();
#endif
