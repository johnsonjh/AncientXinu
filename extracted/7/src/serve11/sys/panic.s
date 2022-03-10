/* panic.s - panic, panic0, _panic */

DISABLE	=	340			/ PS to	disable	interrupts
ENABLE	=	000			/ PS to	enable interrupts

	.globl	panic, panic0, _panic, kernstk
/*------------------------------------------------------------------------
/*  panic0 -- entry point for "branch to zero" error; never called
/*------------------------------------------------------------------------
panic0:
	mfps	kernstk-2		/ save old ps for message
	mtps	$DISABLE		/ disable interrupts
	mov	sp,kernstk		/ save old sp for message
	mov	$kernstk-2,sp		/ use kernel stack for printing
	mov	$panmsg1,-(sp)		/ push address of message
	jsr	pc,_kprintf		/ print	message
	halt
	jmp	start			/ restart system if user continues

/*------------------------------------------------------------------------
/*  _panic  --  entry point when called by user programs
/*------------------------------------------------------------------------
_panic:					/ Note: "_panic" pushes the
					/ PC/PS to simulate a trap so it
					/ can flow into "panic" (the
					/ trap entry point, below)
	mfps	pansav			/ save old PS
	mtps	$DISABLE		/ disable interrupts
	mov	2(sp),pmsguser		/ address of users message
	mov	(sp),-(sp)		/ push ret. address and	user's
	mov	pansav,2(sp)		/  PS for trap simulation
	mtps	$DISABLE+8		/ put "user trap" code in PS

/*------------------------------------------------------------------------
/*  panic  -- entry point for traps and exceptions only; not called
/*------------------------------------------------------------------------
panic:
	mfps	pansav			/ save trap type code from ps
	mov	2(sp),kernstk		/ push on kernel stack:	old ps
	mov	(sp),kernstk-2		/	old pc
	mov	sp,kernstk-4		/	old sp (from before trap)
	add	$4,kernstk-4
	mov	$kernstk-4,sp
	mov	r5,-(sp)		/	old r5
	mov	r4,-(sp)		/	old r4
	mov	r3,-(sp)		/	old r3
	mov	r2,-(sp)		/	old r2
	mov	r1,-(sp)		/	old r1
	mov	r0,-(sp)		/	old r0
	jsr	pc,sizmem		/ size up memory (needed later)

/ set up call to kprintf to print register and stack dump

	mov	kernstk,-(sp)		/ user's ps
	mov	kernstk-2,-(sp)		/ user's pc
	mov	pansav,-(sp)		/ address of panic message
	bic	$177760,(sp)		/  (mask off type&use as index
	mov	(sp),-(sp)		/ panic	trap type
	asl	2(sp)			/ compute message address from type
	add	$pmsglist,2(sp)
	mov	*2(sp),2(sp)		/ go through vector to message
	mov	$panmsg2,-(sp)		/ push format address
	jsr	pc,_kprintf		/ print	message
	add	$10.,sp			/ pop off args to printf, leaving
					/   saved regs,	even though printed

/ dump stack as long as sp was valid

	mov	kernstk-4,r3		/ r3 = addr. user's stack before trap
	sub	$2,sp			/ save space for arg to	printf
	mov	$panmsg3,-(sp)		/ format arg for printf
	mov	$6,r4			/ count	of stack items to print
panloop:
	cmp	r3,_maxaddr		/ avoid	references beyond valid
	bhi	pandone			/  memory addresses
	mov	(r3),2(sp)		/ insert value to print	as arg.
	jsr	pc,_kprintf		/ print	one value
	dec	r4			/ decrement counter of # to print
	ble	pandone			/ stop if enough have been printed
	add	$2,r3			/ otherwise, move along	the stack
	jbr	panloop

/ clean	up and restore the state so user can continue after halt

pandone:
	add	$4,sp			/ pop off the printf args
	mov	(sp)+,r0		/ restore registers
	mov	(sp)+,r1
	mov	(sp)+,r2
	mov	(sp)+,r3
	mov	(sp)+,r4
	mov	(sp)+,r5
	mov	(sp)+,sp		/ restore user's stack pointer
	halt				/ user probably	won't continue
	mov	kernstk,-(sp)		/ but we should	restore	PS and PC
	mov	kernstk-2,-(sp)		/ in case of ODT "P" command,
	rtt				/ especially for call to _panic
	.data
pansav:
.	=	.+2^.			/ word for saving state	info.
panmsg1:
	.byte	12,12,'P,'a,'n,'i,'c,':,' ,'b,'r,'a,'n,'c,'h,' ,'t,'o
	.byte	' ,'l,'o,'c,'.,' ,'0,',,' ,'p,'s,'=,'%,'o,' ,'s,'p,'=,'%
	.byte	'o,12,12,0
panmsg2:
	.byte	12,12,'P,'a,'n,'i,'c,':,' ,'t,'r,'a,'p,' ,'t,'y,'p,'e
	.byte	' ,'%,'d,' ,'(,'%,'s,'),12
	.byte	'S,'t,'a,'t,'e,':,' ,'p,'c,'=,'%,'o,' ,'p,'s,'=,'%,'o
	.byte	' ,'r,'0,'=,'%,'o
	.byte	' ,'r,'1,'=,'%,'o
	.byte	' ,'r,'2,'=,'%,'o
	.byte	' ,'r,'3,'=,'%,'o
	.byte	' ,'r,'4,'=,'%,'o
	.byte	' ,'r,'5,'=,'%,'o,12
	.byte	'S,'t,'a,'c,'k,':,' ,'(,'t,'o,'p,' ,'a,'t,' ,'%,'o,'),12,0
panmsg3:
	.byte	'%,'o,12,0
	.even
pmsglist:
	pmsg0
	pmsg1
	pmsg2
	pmsg3
	pmsg4
	pmsg5
	pmsg6
	pmsg7
pmsguser:
	pmsg0				/ Gets overwritten on panic call
pmsg0:
	.byte	'B,'u,'s,' ,'e,'r,'r,'o,'r,0
pmsg1:
	.byte	'I,'l,'l,'e,'g,'a,'l,' ,'i,'n,'s,'t,'r,'u,'c,'t,'i,'o,'n,0
pmsg2:
	.byte	'B,'P,'T,' ,'o,'r,' ,'T,'-,'b,'i,'t,0
pmsg3:
	.byte	'I,'O,'T,0
pmsg4:
	.byte	'P,'o,'w,'e,'r,' ,'f,'a,'i,'l,0
pmsg5:
	.byte	'E,'M,'T,0
pmsg6:
	.byte	'T,'r,'a,'p,' ,'i,'n,'s,'t,'r,'u,'c,'t,'i,'o,'n,0
pmsg7:
	.byte	'U,'n,'k,'n,'o,'w,'n,' ,'d,'e,'v,'i,'c,'e,0

/* Stack used by kernel at startup and to handle panic messages.	*/

	.even
.	=	.+300.			/ decimal bytes	in kernel stack
kernstk:
.	=	.+2			/ Otherwise kernstk overlaps
					/ next symbol
