|* setclkr.s - setclkr *|

CVECTPC	=	0x0078			| clock interrupt vector address
CDATVEC=	0x800000		| clock data register address
CCSRVEC	=	0x800002		| clock command and status register
DISABLE	=	0x2700			| PS that disables all interrupts
ENABLE	=	0x2500			| PS that enables clock interrupts
COUNT	=	30000			| Number of times to loop

|*------------------------------------------------------------------------
|* setclkr  --  set cklruns to 1 iff real-time clock exists, 0 otherwise
|*------------------------------------------------------------------------
	.data
	.text
	.globl	_setclkr
_setclkr:

	movl	d1,sp@-			| save register	used
	clrl	_clkruns		| initialize for no clock
	movl	CVECTPC,sp@-		|   on caller's stack
	movl	#setint,CVECTPC
	movl	#COUNT,d1		| initialize counter for loop
|	reset				| clear other interrupts, if any

	movw	#0xff02,CCSRVEC		| Set data pointer to mode register 2
	movw	#0x0b22,CDATVEC		| Set mode D, tc toggled, bcd count
	movw	#0x03c0,CDATVEC		| Load register, 60 Hz
	movw	#0xff22,CCSRVEC		| Arm (start) timer #2

	movl	#ENABLE, sr		| allow	interrupts
setloop:
	dbeq	d1, setloop		| loop COUNT times waiting for interrupt
	movw	#DISABLE,sr		| disbale all interrupts
	bra	setdone			| no interrupt occurred, so quit
setint:
	movw	#DISABLE,sr
	movl	#1,_clkruns		| clock interrupt jumps here
	addql	#6,sp			| pop pc|ps pushed by interrupt

setdone:
	movl	sp@+,CVECTPC		| restore old interrupt vector
	movl	sp@+,d1			| restore register
	rts				| return to caller
	.data
