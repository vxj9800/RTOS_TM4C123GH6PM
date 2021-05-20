; Author: Vatsal Asitkumar Joshi
; Date: Oct 2nd, 2019
; This is an assembly code for waitMicrosecond() function.
;
; "If you are done writing the code, now is a good time to debug it."
;

;-----------------------------------------------------------------------------
; Device includes, defines, and assembler directives
;-----------------------------------------------------------------------------

	.def actPriv
	.def setSp
	.def getSp
	.def actPsp
	.def setPsp
	.def getPsp
	.def pushR42R11
	.def popR42R11
	.def getSvcNo
	.def getR0
	.def getR1

;-----------------------------------------------------------------------------
; Register values and large immediate values
;-----------------------------------------------------------------------------

.thumb
.const

;-----------------------------------------------------------------------------
; Subroutines
;-----------------------------------------------------------------------------

.text

actPriv:			MRS		R0, CONTROL
					ORR		R0, R0, #0x1
					MSR		CONTROL, R0
					BX		LR
setSp:				MOV		SP, R0
					BX		LR
getSp:				MOV		R0, SP
					BX		LR
actPsp:				MRS		R0, CONTROL
					ORR		R0, R0, #0x2
					MSR		CONTROL, R0
					MRS		SP, MSP
					BX		LR
setPsp:				MSR		PSP, R0
					BX		LR
getPsp:				MRS		R0, PSP
					BX		LR
pushR42R11:			STMDB	R0!, {R4,R5,R6,R7,R8,R9,R10,R11}
					;SUB		R0, #32
					BX		LR
popR42R11:			LDMFD	R0!, {R4,R5,R6,R7,R8,R9,R10,R11}
					;ADD		R0, #32
					BX		LR
getSvcNo:			MRS		R0, PSP
					LDR		R0, [R0,#24]
					SUB		R0, #2
					LDRB	R0, [R0]
					BX		LR
getR0:				MRS		R0, PSP
					LDR		R0, [R0]
					BX		LR
getR1:				MRS		R0, PSP
					LDR		R0, [R0,#4]
					BX		LR
.endm
