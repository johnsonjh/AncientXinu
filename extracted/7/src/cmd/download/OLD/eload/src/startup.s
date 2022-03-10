/* startup.s - start */

DISABLE	=	340			/ PS to disable interrupts
ENABLE	=	0			/ PS to enable interrupts
STKADDR	= 	017776			/ Stack address to use
reset   = 0000005			/ LSI 11 opcodes not recognized
mfps	= 0106700 ^ tst			/ by the assembler
mtps	= 0106400 ^ tst

/*----------------------------------------------------------------------
/*  Etherload startup header -- branches to main program after
/*            getting the stack pointer in place for C run-time
/*----------------------------------------------------------------------
	.globl	estart, start, _ecopy
	jmp	estart
args:
.	=	12.^.			/ space for args
start:
	mtps	$DISABLE		/ Disable interrupts
	reset				/ Clear the bus
estart:
	mtps	$DISABLE		/ Disable interrupts
	mov	$STKADDR,sp		/ set up stack pointer in 1st page
	jsr	pc,_ecopy		/ start eload C program
	halt				/ halt if program returns
	jbr	estart			/ rerun if user types P
