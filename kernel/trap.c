#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#include "clint.h"
#include "console.h"
#include "cpu.h"
#include "lib.h"
#include "plic.h"
#include "printf.h"

uintptr_t m_trap (uintptr_t epc,
            uintptr_t tval,
            uintptr_t cause,
            uintptr_t hart,
            uintptr_t status,
            TrapFrame *frame)
{
    const bool is_async = (cause >> 63 & 1);
    const uintptr_t cause_num = cause & 0xfff;
    uintptr_t return_pc = epc;
    uint32_t plic_int, plic_c;

    if (is_async)
    {
        switch (cause_num)
        {
            case 3:
                printf ("Machine software interrupt CPU#%"PRIuPTR"\n", hart);
                break;
            case 7:
                // timer int -> extend 1 sec
                CLINT_SET_TIMEOUT (hart, CLINT_CLOCK_FREQ);
                break;
            case 11:
                plic_int = plic_next ();
                if (plic_int == 0)
                {
                    break;
                }
                switch (plic_int)
                {
                    case 10:
                        // uart interrupt
                        switch (plic_c = console_getc ())
                        {
                            case 8:
                                printf ("%c %c", 8, 8);
                                break;
                            case 10:
                            case 13:
                                printf ("\n");
                                break;
                            default:
                                printf ("%c", plic_c);
                                break;
                        }
                        break;
                    default:
                        printf ("Non-UART external interupt: %d\n", plic_int);
                        break;
                }
                plic_complete (plic_int);
                break;
            default:
                panic ("Unhandled async trap CPU#%"PRIuPTR"\n -> %"PRIuPTR"\n", hart, cause_num);
                break;
        }
    }
    else
    {
        switch (cause_num)
        {
            case 2:
                panic ("Illegal instruction CPU#%"PRIuPTR" -> 0x%"PRIxPTR": 0x%"PRIxPTR"\n", hart, epc, tval);
                break;
            case 3:
                return_pc += 4;
                break;
            case 8:
                printf ("E-call from User mode! CPU#%"PRIuPTR" -> 0x%"PRIxPTR"\n", hart, epc);
                return_pc += 4;
                break;
            case 9:
                printf ("E-call from Supervisor mode! CPU#%"PRIuPTR" -> 0x%"PRIxPTR"\n", hart, epc);
                return_pc += 4;
                break;
            case 11:
                panic ("E-call from Machine mode! CPU#%"PRIuPTR" -> 0x%"PRIxPTR"\n", hart, epc);
                break;
            case 12:
                printf ("Instruction page fault CPU#%"PRIuPTR" -> 0x%"PRIxPTR": 0x%"PRIxPTR"\n", hart, epc, tval);
		return_pc += 4;
                break;
            case 13:
                printf ("Load page fault CPU#%"PRIuPTR" -> 0x%"PRIxPTR": 0x%"PRIxPTR"\n", hart, epc, tval);
		return_pc += 4;
                break;
            case 15:
                printf ("Store page fault CPU#%"PRIuPTR" -> 0x%"PRIxPTR": 0x%"PRIxPTR"\n", hart, epc, tval);
                return_pc += 4;
                break;
            default:
                panic ("Unhandled sync trap CPU#%"PRIuPTR" -> %"PRIuPTR"\n", hart, cause_num);
                break;
        }
    }
    return return_pc;
}

