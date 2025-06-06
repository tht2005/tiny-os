#include "console.h"
#include "uart.h"
#include <assert.h>

void kmain ()
{
    console_init ();
    console_puts ("Hello, World!\n");
    while(1)
    {
        char c = uart_getc();
        console_puts ("You just press ");
        switch (c)
        {
            case 0x1b:
                c = uart_getc();
                if (c == '[') {
                    switch (uart_getc()) {
                        case 'A':
                            console_puts("up arrow");
                            break;
                        case 'B':
                            console_puts("down arrow");
                            break;
                        case 'C':
                            console_puts("right arrow");
                            break;
                        case 'D':
                            console_puts("left arrow");
                            break;
                        default:
                            break;
                    }
                }
                break;
            case '\n':
            case '\r':
                console_puts ("enter");
                break;
            case ' ':
                console_puts ("space");
                break;
            case '\t':
                console_puts ("tab");
                break;
            default:
                console_putc (c);
                break;
        }
        console_puts (".\n");
    }
}
