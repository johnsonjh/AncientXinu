|* clkint.s -  clkint

|*------------------------------------------------------------------------
|* clkint  --  real-time clock interrupt service routine 
|*------------------------------------------------------------------------
	.text
	.globl	_clkint
_clkint:
	subql	#1,_count6		| Is this the 6th interrupt?
	bpl	clret			|  no => return
	movl	#6,_count6		|  yes=> reset counter&continue
	tstl     _defclk		| Are clock ticks deferred?
	beq     notdef			|  no => skip down
	addql	#1,_clkdiff		|  yes=> count in clkdiff and
	rte				|        return quickly
notdef:	
	tstl     _slnempty		| Is sleep queue nonempty?
	beq     clpreem			|  yes=> skip down
	subql	#1,_sltop		|  no => decrement delta key
	bpl     clpreem			|        of first process,
	moveml	#0xc0c0,sp@-		| Save d0,d1,a0,a1 - C compiler does not
	jsr	_wakeup			|     
	moveml	sp@+,#0x0303		| Restore d0,d1,a0,a1
clpreem:
	subql	#1,_preempt		| Decrement preemption counter
	bpl	clret			|   and call resched if it
	moveml	#0xc0c0,sp@-
	jsr	_resched
	moveml	sp@+,#0x0303
clret:
	rte				| Return from interrupt
