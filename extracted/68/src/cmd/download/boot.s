| boot.s  -  bootstrap loader

| boot  --  loads the fast loader into the high portion of memory;
|           when executed, register d0 contains the size of the fast loader
|	    in bytes

LENGTH	  = 0xffe			| Address where length is hidden
UARTDATA  = 0x600004			| Port B Uart Data   
UARTSTAT  = 0x600006			| Port B Uart Status 
MAXADDR	  = 0x40000			| Highest memory location
RETADDR	  = 0x200c82
RXFULL    = 0				| Receiver Full Bit

	.data
	.text
	.globl	_boot
_boot:
	movw	LENGTH, d0
	movl	#MAXADDR, a0
getchar:
	btst	#RXFULL, UARTSTAT	| Wait until a character is sent
	beq	getchar
	movb	UARTDATA, a0@-		| Move character into high memory
	subql	#1, d0			| Decrement count of incoming chs
	bne	getchar			| If more characters, go get them
	movb	#0x24, 0x600000
	jmp	RETADDR			| Return to monitor
