#include <string.h>
#include <global.h>
#include <register.h>
#include <intc.h>


#define INTC_INTR_ENTRY_SIZE    (sizeof(IpIsrFunc))


/* IntcIntrEntry a segment of memory for interrupt entry table */
IpIsrFunc IntcIntrEntryTable[INTC_NUM_OF_CHANNELS];

//void IntcIsrFIQ(void) ALWAYS_INLINE;
void IntcIsrFIQ(void)
{
    register IpIsrFunc *p;

    for (p = (IpIsrFunc *) REG_FIQE; p; p = (IpIsrFunc *) REG_FIQE)
    {
        (*p)();
    }
}

//void IntcIsrIRQ(void) ALWAYS_INLINE;
void IntcIsrIRQ(void)
{
    register IpIsrFunc *p;

    for (p = (IpIsrFunc *) REG_IRQE; p; p = (IpIsrFunc *) REG_IRQE)
    {
        (*p)();
    }
}

void IntcInit(void)
{
    dwrd v;

    // set entry table as all 0's
    memset(IntcIntrEntryTable, 0, sizeof(IntcIntrEntryTable));

    // disable all interrupts
    REG_IMR = 0xFFFFFFFF;

    // entry table base register
    v = (dwrd) IntcIntrEntryTable;
    v &= 0xFFFFFFFC;

    if (INTC_INTR_ENTRY_SIZE == 4)
        ;
    else if (INTC_INTR_ENTRY_SIZE == 8)
    {
        v |= 1;
    }
    else if (INTC_INTR_ENTRY_SIZE == 16)
    {
        v |= 2;
    }
    else
    {
        v |= 3;
    }

    REG_EBA = v;
}

byte IntcRegisterISR(byte bChnlNum, IntcIntrPrio nPrio, IpIsrFunc isr)
{

    if (bChnlNum >= INTC_NUM_OF_CHANNELS || !isr)
    {
        return (0);
    }

    // interrupt entry
    IntcIntrEntryTable[bChnlNum] = isr;

    // source control register
    {
        dwrd r, q;
        dwrd v;

        r = bChnlNum % 4;
        q = bChnlNum / 4;

        // r-m-w
        v = REG_SCR(q);
        v &= (~(7 << (r * 8)));     /* clear corresponding priority */
        v |= (nPrio << (r * 8));    /* set corresponding priority */
        REG_SCR(q) = v;
    }

    return (1);
}

void IntcUnregisterISR(byte bChnlNum)
{
    IntcIntrEntryTable[bChnlNum] = NULL;
}

void IntcEnableIntr(byte bChnlNum)
{
    REG_MECR = 1 << bChnlNum;
}

void IntcDisableIntr(byte bChnlNum)
{
    REG_IMR = 1 << bChnlNum;
}

void IntcDisableAllIntrs(void)
{
    REG_IMR = 0xFFFFFFFF;
}

//void IntcClearIntr(byte bChnlNum) ALWAYS_INLINE;
void IntcClearIntr(byte bChnlNum)
{
    REG_FIQ = 1 << bChnlNum;
    REG_IRQ = 1 << bChnlNum;
    // It will be okay to clear these two registers
    // at the same time.
}

void IntcClearAllIntrs(void)
{
    REG_FIQ = 0xFFFFFFFF;
    REG_IRQ = 0xFFFFFFFF;
}
