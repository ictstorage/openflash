

    AREA Vect, CODE, READONLY

    IMPORT  Reset_Handler
    IMPORT  Undefined_Handler
    IMPORT  SWI_Handler
    IMPORT  Prefetch_Handler
    IMPORT  Abort_Handler
    IMPORT  Reserved_Handler
    IMPORT  IRQ_Handler
    IMPORT  FIQ_Handler

    EXPORT  DbgCode


    ENTRY

    LDR PC, Reset_Addr
    LDR PC, Undefined_Addr
    LDR PC, SWI_Addr
    LDR PC, Prefetch_Addr
    LDR PC, Abort_Addr
    LDR PC, Reserved_Addr
    LDR PC, IRQ_Addr
    LDR PC, FIQ_Addr

Reset_Addr      DCD Reset_Handler
Undefined_Addr  DCD Undefined_Handler
SWI_Addr        DCD SWI_Handler
Prefetch_Addr   DCD Prefetch_Handler
Abort_Addr      DCD Abort_Handler
Reserved_Addr   DCD Reserved_Handler
IRQ_Addr        DCD IRQ_Handler
FIQ_Addr        DCD FIQ_Handler


DbgCode             DCD     0
Reserved1           DCD     0
Reserved2           DCD     0
Reserved3           DCD     0


    END

