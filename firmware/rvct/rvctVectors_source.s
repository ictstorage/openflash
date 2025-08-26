

	AREA Vect, CODE, READONLY


	ENTRY

	LDR	PC, Reset_Addr
	LDR	PC, Undefined_Addr
	LDR	PC, SWI_Addr
	LDR	PC, Prefetch_Addr
	LDR	PC, Abort_Addr
	NOP
	LDR	PC, IRQ_Addr
	LDR	PC, FIQ_Addr



	IMPORT	FIQ_Handler 
	IMPORT	IRQ_Handler
	IMPORT	Reset_Handler

Reset_Addr	DCD	Reset_Handler
Undefined_Addr	DCD	Undefined_Handler
SWI_Addr	DCD	SWI_Handler
Prefetch_Addr	DCD	Prefetch_Handler
Abort_Addr	DCD	Abort_Handler
IRQ_Addr	DCD	IRQ_Handler
FIQ_Addr	DCD	FIQ_Handler


;Reset_Handler
;	B	Reset_Handler

Undefined_Handler
	B	Undefined_Handler

SWI_Handler
	B	SWI_Handler     

Prefetch_Handler
	B	Prefetch_Handler

Abort_Handler
	B	Abort_Handler

;IRQ_Handler
;	B	IRQ_Handler

;FIQ_Handler
;	B	FIQ_Handler


;
; TODO : put more parameterized tables here in the followings
;


;
; reserved a 256 16-bit words for SATA's identify device
;
#ifdef WITH_SATA_ID_TABLE

#include "defSATAID.tbl"


#endif
        


	END

