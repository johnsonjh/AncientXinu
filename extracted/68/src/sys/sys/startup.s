|* startup.s - start */

DISABLE	=	0x2700			| PS to disable interrupts

|*------------------------------------------------------------------------
|*  Xinu system entry point -- first location beyond interrupt vectors
|*------------------------------------------------------------------------
	.data
	.text
	.globl	start
	.globl	_exit
start:
	movw	#DISABLE,sr	 	| Disable interrupts
	movl	#kernstk,sp		| set up temporary stack pointer
	jsr	_sizmem			| _maxaddr set to max address
	movl	_maxaddr,sp		| switch stack to high memory
	tstl	sp@-			| leave space for MAGIC word
	jsr	_loinit
|	reset				| reset bus
	jmp	_nulluser		| Jump to C startup routine
_exit:	jmp	_exit
