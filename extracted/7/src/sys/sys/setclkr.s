/* setclkr.s - setclkr */

CVECTPC	=	100			/ clock interrupt vector address
CVECTPS	=	102			/  "      "         "      "
DISABLE	=	340			/ PS that disables interrupts
ENABLE	=	000			/ PS that enables interrupts
COUNT	=	32700.			/ Times to loop (in decimal)

/*------------------------------------------------------------------------
/* setclkr  --  set cklruns to 1 iff real-time clock exists, 0 otherwise
/*------------------------------------------------------------------------
	.globl	_setclkr
_setclkr:
	mov	r1,-(sp)		/ save register	used
	clr	_clkruns		/ initialize for no clock
	mov	*$CVECTPS,-(sp)		/ save clock interrupt vector
	mov	*$CVECTPC,-(sp)		/   on caller's stack
	mov	$DISABLE,*$CVECTPS	/ set up new interrupt vector
	mov	$setint,*$CVECTPC
	mov	$COUNT,r1		/ initialize counter for loop
	reset				/ clear other interrupts, if any
	mtps	$ENABLE			/ allow	interrupts
setloop:
	dec	r1			/ loop COUNT times waiting for
	bpl	setloop			/   a clock interrupt
	mtps	$DISABLE		/ no interrupt occurred, so quit
	br	setdone
setint:
	inc	_clkruns		/ clock interrupt jumps here
	add	$4,sp			/ pop pc/ps pushed by interrupt
setdone:
	mov	(sp)+,*$CVECTPC		/ restore old interrupt vector
	mov	(sp)+,*$CVECTPS
	mov	(sp)+,r1		/ restore register
	rts	pc			/ return to caller
