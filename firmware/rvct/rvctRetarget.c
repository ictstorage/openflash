//#include <stdio.h>
#include <rt_misc.h>

#include <global.h>

#ifdef __thumb
    /* Thumb Semihosting SWI */
    #define SemiSWI 0xAB
#else
    /* ARM Semihosting SWI */
    #define SemiSWI 0x123456
#endif

/* Write a character */
__swi(SemiSWI) void _WriteC(unsigned op, char *c);
#define WriteC(c) _WriteC (0x3,c)

/* Exit */
__swi(SemiSWI) void _Exit(unsigned op, unsigned except);
#define Exit() _Exit (0x18,0x20026)


struct __FILE
{
    int handle;   /* Add whatever you need here */
};
//  #if (!defined (SWIFUNC) && !defined (UARTFUNC))
//  FILE __stdout;
//  #endif

extern unsigned int bottom_of_heap;     /* defined in rvctHeap.s */

//  #if (!defined (SWIFUNC) && !defined (UARTFUNC))
//  int fputc(int ch, FILE *f)
//  {
//      return ch;
//  }
//  #endif
//
//  int ferror(FILE *f)
//  {
//      return EOF;
//  }

void _sys_exit(int return_code)
{
    Exit();         /* for debugging */

label:
    goto label; /* endless loop */
}

void _ttywrch(int ch)
{

}


__value_in_regs struct __initial_stackheap __user_initial_stackheap(
    unsigned R0, unsigned SP, unsigned R2, unsigned SL)
{
    struct __initial_stackheap config = {0};

    config.heap_base = (unsigned int) &bottom_of_heap; // defined in rvctHeap.s
                                                       // placed by scatterfile
    config.stack_base = SP;   // inherit SP from the execution environment

    return config;
}

/*
Below is an equivalent example assembler version of __user_initial_stackheap.

It will be entered with the value of the stackpointer in r1 (as set in init.s),
this does not need to be changed and so can be passed unmodified out of the
function.

    IMPORT bottom_of_heap
    EXPORT __user_initial_stackheap

__user_initial_stackheap
    LDR   r0,=bottom_of_heap
    MOV   pc,lr
*/

