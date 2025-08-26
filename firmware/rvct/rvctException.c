#include <global.h>

void __irq Undefined_Handler(void)
{
    while (1)
    {
        ;
    }
}

void __irq SWI_Handler(void)
{
    while (1)
    {
        ;
    }
}

void __irq Prefetch_Handler(void)
{
    while (1)
    {
        ;
    }
}

void __irq Abort_Handler(void)
{
#ifdef END2END_DP
    // for E2E
    IntcAbort_cpu2();
#else
    while (1)
    {
        ;
    }
#endif
}

void __irq Reserved_Handler(void)
{
    while (1)
    {
        ;
    }
}
