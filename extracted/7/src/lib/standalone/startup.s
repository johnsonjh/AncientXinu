/* startup.s - start, _disable, _enable */ /* standalone version */

DISABLE	=	340			/ PS to disable interrupts
ENABLE	=	0			/ PS to enable interrupts
EXCPPC	=	4			/ exception vector address for
EXCPPS	=	6			/ "memory out of range" errors
MAXADDR	= 0157776			/ maximum valid memory address
reset   = 0000005			/ LSI 11 opcodes not recognized
mfps	= 0106700 ^ tst			/ by the assembler
mtps	= 0106400 ^ tst

/*----------------------------------------------------------------------
/*  Stand-alone startup header -- branches to main program after
/*            getting the stack pointer in place for C run-time
/*  Features: sizes memory and starts SP at highest address.
/*            preserves interrupt vectors in case they have
/*            been set statically.
/*            leaves interrupts disabled when program starts
/*            (call enable/disable to enable or disable them
/*            while running)
/*            Will halt if user program returns; type P to run again
/*----------------------------------------------------------------------
	.globl	start, _restart, _main, _disable, _enable
_restart:
start:
	mtps	$DISABLE		/ Disable interrupts
	reset				/ Clear the bus
	mov	$kernstk,sp		/ set up temporary stack pointer
	mov	*$EXCPPC,r3		/ save old contents of
	mov	*$EXCPPS,r4		/  exception vector
	mov	$siztrap,*$EXCPPC	/ set up vector to catch memory
	mov	$DISABLE,*$EXCPPS	/ exceptions; disable interrupts
	mov	$MAXADDR,r0		/ set r0 to highest possible loc
sizloop:
	mov	(r0),(r0)		/ reference what r0 points to
	mov	r4,*$EXCPPS		/ no fault, so memory exists
	mov	r3,*$EXCPPC		/ restore exception vectors
	mov	r0,sp			/ set stack pointer
	clr	(sp)			/ clear argv
	clr	-(sp)			/ clear argc
	jsr	pc,_main		/ start main program
	halt				/ halt if program returns
	jbr	start			/ rerun if user types P

siztrap:
	add	$4,sp			/ pop interrupted PC and PS
	sub	$8.,r0			/ move to next lower address
	jbr	sizloop			/ try again

_disable:
	mtps	$DISABLE		/ externally callable routine to
	rts	pc			/   disable interrupts

_enable:
	mtps	$ENABLE			/ externally callable routine to
	rts	pc			/   enable interrupts

	.data
.	=	.+6			/ temporary stack space for trap
kernstk:				/  while sizing memory
.	=	.+2
