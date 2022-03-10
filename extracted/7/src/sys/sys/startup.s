/* startup.s - start */

DISABLE	=	340			/ PS to disable interrupts

/*------------------------------------------------------------------------
/*  Xinu system entry point -- first location beyond interrupt vectors
/*------------------------------------------------------------------------
	.globl	start, _restart
_restart:				/ a name used by C for reboot
start:
	mtps	$DISABLE		/ Disable interrupts
	mov	$kernstk,sp		/ set up temporary stack pointer
	jsr	pc,sizmem		/ _maxaddr set to max address
	mov	_maxaddr,sp		/ switch stack to high memory
	tst	-(sp)			/ leave space for MAGIC word
	reset				/ reset bus
	clr	r5			/ clear initial r5 for debugging
	clr	r4
	jmp	_nulluser		/ Jump to C startup routine
