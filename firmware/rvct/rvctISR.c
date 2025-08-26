#include <global.h>
#include <intc.h>

void __irq FIQ_Handler(void)
{
    IntcIsrFIQ();
}

void __irq IRQ_Handler(void)
{
    IntcIsrIRQ();
}

