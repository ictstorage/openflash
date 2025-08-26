
	PRESERVE8
	AREA	sysCPU_A, CODE, READONLY

;*********************************************************************************************************
;
; Prototypes :     unsigned int  sysCPUSaveSR(void);
;                  void          sysCPURestoreSR(cpu_sr);
;
;
; Usage:
;                 void function (void *p_arg)
;                 {
;                          :
;                          :
;                     sr = sysCPUSaveSR();
;                          :
;                          :
;                     sysCPURestoreSR(sr); 
;                          :
;                          :
;                 }
;
;*********************************************************************************************************


NO_INT      EQU     0xC0                         ; Mask used to disable interrupts (Both FIR and IRQ)

Mode_USR	EQU	0x10
Mode_SYS	EQU	0x1F

I_Bit		EQU	0x80 ; when I bit is set, IRQ is disabled
F_Bit		EQU	0x40 ; when F bit is set, FIQ is disabled


	EXPORT sysCPUSaveSR
	EXPORT sysCPURestoreSR
	EXPORT sysCPUSysModeEnaIntr
	EXPORT sysCPUSysModeDisIntr
;	EXPORT sysCPUDrainWriteBuffer


sysCPUSaveSR FUNCTION
        MRS     R0,CPSR                     ; Set IRQ and FIQ bits in CPSR to disable all interrupts
        ORR     R1,R0,#NO_INT
        MSR     CPSR_c,R1
        MRS     R1,CPSR                     ; Confirm that CPSR contains the proper interrupt disable flags
        AND     R1,R1,#NO_INT
        CMP     R1,#NO_INT
        BNE     sysCPUSaveSR                ; Not properly disabled (try again)
        ;MOV     PC,LR                       ; Disabled, return the original CPSR contents in R0
        BX		LR

	ENDFUNC


sysCPURestoreSR FUNCTION
        MSR     CPSR_c,R0
        ;MOV     PC,LR
        BX		LR

	ENDFUNC


sysCPUSysModeEnaIntr FUNCTION
	MSR	CPSR_c, #Mode_SYS
	;MOV	pc, lr
	BX		LR
	ENDFUNC

sysCPUSysModeDisIntr FUNCTION
	MSR	CPSR_c, #Mode_SYS:OR:I_Bit:OR:F_Bit
	;MOV	pc, lr
	BX		LR
	ENDFUNC


;for ARM968 only
;sysCPUDrainWriteBuffer FUNCTION
;	MCR	p15,	0,	r0,	c7,	c10,	4
;	MOV	pc, lr
;	ENDFUNC

	END
