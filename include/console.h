#ifndef __CONSOLE_H
#define __CONSOLE_H
void console_init ();
void console_putc (char c);
int console_getc ();
void console_puts (const char *str);
#endif
