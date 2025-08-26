;	include config_a.h

	PRESERVE8
	AREA    Init, CODE, READONLY

;
; Standard definitions of mode bits and interrupt (I & F) flags in PSRs
;
Mode_USR	EQU	0x10
Mode_FIQ	EQU	0x11
Mode_IRQ	EQU	0x12
Mode_SVC	EQU	0x13
Mode_ABT	EQU	0x17
Mode_UND	EQU	0x1B
Mode_SYS	EQU	0x1F ; available on ARM Arch 4 and later

I_Bit		EQU	0x80 ; when I bit is set, IRQ is disabled
F_Bit		EQU	0x40 ; when F bit is set, FIQ is disabled

;
; Amount of memory (in bytes) allocated for stacks
;
FIQ_Stack_Size	EQU	0
IRQ_Stack_Size	EQU	1024
ABT_Stack_Size	EQU	0	;this mode isn't used
UND_Stack_Size	EQU	0	;this mode isn't used
SVC_Stack_Size	EQU	0	;this mode isn't used

; Add lengths >0 for FIQ_Stack, ABT_Stack, UND_Stack if you need them.
; XXX_Stack will be loaded as immediate values.
; XXX_Stack must be 8 byte aligned.

FIQ_Stack	EQU	0
IRQ_Stack	EQU	FIQ_Stack + FIQ_Stack_Size
ABT_Stack	EQU	IRQ_Stack + IRQ_Stack_Size
UND_Stack	EQU	ABT_Stack + ABT_Stack_Size
SVC_Stack	EQU	UND_Stack + UND_Stack_Size
USR_Stack	EQU	SVC_Stack + SVC_Stack_Size


;       ENTRY
	EXPORT  Reset_Handler

Reset_Handler

;
; Initialize stack pointer registers
;

	IMPORT	top_of_stacks       ; defined in rvctStack.s and located by scatter file

	LDR	r0, =top_of_stacks

	MSR	CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit	; No interrupts
	SUB	r1, r0, #FIQ_Stack
	MOV	sp, r1

	MSR	CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit	; No interrupts
	SUB	r1, r0, #IRQ_Stack
	MOV	sp, r1

	MSR	CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit	; No interrupts
	SUB	r1, r0, #ABT_Stack
	MOV	sp, r1

	MSR	CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit	; No interrupts
	SUB	r1, r0, #UND_Stack
	MOV	sp, r1

	MSR	CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit	; No interrupts
	SUB	r1, r0, #SVC_Stack
	MOV	sp, r1

; enable ARM-968 write buffer
;	MRC	p15,	0,	r0,	c1,	c0,	0	;read Control Register
;	ORR	r1,	r0,	#8				;bit3=1: enable CPU write buffer
;	MCR	p15,	0,	r1,	c1,	c0,	0	;write Control Register

; enable ARM-968 TCM
	MRC	p15,	0,	r12,	c0,	c0,	2	;read TCM Size Register
	MRC	p15,	0,	r0,	c1,	c0,	0	;read Control Register
	ORR	r1,	r0,	#0x1000				;bit12=1: enable ITCM
	MCR	p15,	0,	r1,	c1,	c0,	0	;write Control Register

; disable TCM write buffer
	MCR	p15,	0,	r0,	c7,	c10,	4	;drain write buffer
	MRC	p15,	1,	r0,	c15,	c1,	0
	ORR	r0,	r0,	#0x040000			;#EN_ITCM_STALL_WHEN_WB_NOT_EMPTY
	ORR	r0,	r0,	#0x020000			;#EN_DTCM_STALL_WHEN_WB_NOT_EMPTY
	MCR	p15,	1,	r0,	c15,	c1,	0

;
; Now change to System mode and set up System (User) mode stack
;
	MSR	CPSR_c, #Mode_SYS:OR:I_Bit:OR:F_Bit	; No interrupts
	LDR	r0, =top_of_stacks
	SUB	r1, r0, #USR_Stack
	MOV	sp, r1

	MOV	r3, #0x0
	MOV	r4, #0x0
	MOV	r5, #0x0
	MOV	r6, #0x0
	MOV	r7, #0x0
	MOV	r8, #0x0
	MOV	r9, #0x0
	MOV	r10, #0x0
	MOV	r11, #0x0
	MOV	r12, #0x0

	B	COLD_RESET	;alwasys jump to COLD_RESET if ICE download


	IMPORT	__main

COLD_RESET
	MOV	r1, #0x0
	MOV	r2, #0x0
	B	__main	; note use B not BL, because an application will never return this way

	END
