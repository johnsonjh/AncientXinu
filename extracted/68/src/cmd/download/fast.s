|*------------------------------------------------------------------------------
|* fast.s  -  fast loader
|*------------------------------------------------------------------------------

#define	RetVal		d0
#define TempD1		d1
#define	TempD2		d2
#define	TempD3		d3
#define	CheckSum	d4
#define	PackLen		d5
#define	TPackLen	d6
#define BssLen		d7

#define TempA1		a0
#define BaseAddr	a1
#define BufAddr		a2
#define TBufAddr	a3

|*------------------------------------------------------------------------------
|* fast  --  loads the user program into memory;
|*	    Arguments:
|*		USP <- address before fast loader
|*		D7  <- bss length in words
|*------------------------------------------------------------------------------

UARTDATA  = 0x600004			| Port B Uart Data   
UARTSTAT  = 0x600006			| Port B Uart Status 
RXFULL    = 0x00			| Receiver Full Bit
TXEMPTY   = 0x02			| Transmitter Empty Bit
RETADDR	  = 0x200c82
STACKADDR = 0xffc

PACKSIZE  = 0x0400
SOH	  = 0x2f			| '/'
ACK	  = 0x79			| 'y'
NAK	  = 0x6e			| 'n'
ESC	  = 0x1b			| ESCAPE

	.data
	.text
initial:
	movl	STACKADDR, sp
	subl	#PACKSIZE, 	sp
	movl	sp, BufAddr
	subql	#4, sp

getpack:
	movb	#0x73, 0x600000		| print an 's'
	bsr	rawgetc
	cmpb	#SOH,	RetVal		| scan for Start 	(1 byte)
	bne	getpack			| not start of packet, search more
	clrl	CheckSum		| clear checksum value

getbase:			 	| get the base address  (4 bytes)
	bsr	getlsum
	movl	RetVal,	 BaseAddr

getlen:					| get the packet length (4 bytes)
	bsr	getlsum
	movl	RetVal, PackLen

chkrange:				| check the range on the length
	cmpl	#PACKSIZE, PackLen
	bgt	sendnak

copyvars:				| copy length and address
	movl	PackLen, TPackLen	
	beq	getsum
	movl	BufAddr, TBufAddr
getbuf:
	bsr	getcsum
	movb	RetVal, TBufAddr@+
	subql	#1, TPackLen
	bne	getbuf

|-------------------------------------------------------------------------------
| getsum - get transmitted  checksum, total should be 0 
|-------------------------------------------------------------------------------

getsum:
	bsr	getc
	movl	RetVal, TempD1
	asll	#8, TempD1
	bsr	getc
	orl	RetVal, TempD1
	asll	#8, TempD1
	bsr	getc
	orl	RetVal, TempD1
	asll	#8, TempD1
	bsr	getc
	orl	RetVal, TempD1
	andl	TempD1, CheckSum
	beq	copypack

|-------------------------------------------------------------------------------
| sendnak - send a negative acknowledgement and retry
|-------------------------------------------------------------------------------

sendnak:
	movb	#NAK,	RetVal
	bsr	rawputc
	bra	getpack

|-------------------------------------------------------------------------------
| copypack - copy packet from buffer to memory
|-------------------------------------------------------------------------------

copypack:
	movl	BufAddr, TBufAddr
	movl	PackLen, TPackLen
	beq	sendack
cmore:
	movb	TBufAddr@+, BaseAddr@+
	subql	#1, TPackLen
	bne	cmore

|-------------------------------------------------------------------------------
| sendack - send positive acknowledgement
|-------------------------------------------------------------------------------

sendack:
	movb	#ACK, RetVal
	bsr	rawputc
	tstl	PackLen
	bne	getpack
	jmp	RETADDR


|-------------------------------------------------------------------------------
|  rawputc  --  put a character on the line
|-------------------------------------------------------------------------------

rawputc:
	btst	#TXEMPTY, UARTSTAT
	beq	rawputc
	movb	RetVal, UARTDATA
	movb	RetVal, 0x600000
	rts

|-------------------------------------------------------------------------------
|  rawgetc  --  get a character from the line
|-------------------------------------------------------------------------------

rawgetc:
	clrl	RetVal
check1:
	btst	#RXFULL, UARTSTAT
	beq	check1
	movb	UARTDATA, RetVal
	rts

|-------------------------------------------------------------------------------
|  getc  --  get a character from the line, honoring ESCAPE mechanism
|-------------------------------------------------------------------------------

getc:
	bsr	rawgetc
	cmpb	#ESC, RetVal
	bne	ret
	bsr	rawgetc
ret:
	rts

|-------------------------------------------------------------------------------
|  getcsum  --  get a character (1 byte)  and add to checksum
|-------------------------------------------------------------------------------

getcsum:
	bsr	getc
	addb	RetVal, CheckSum
	rts

|-------------------------------------------------------------------------------
|  getwsum  --  get a word (2 bytes) and add to checksum
|-------------------------------------------------------------------------------

getwsum:
	clrl	TempD1
	bsr	getcsum			| get high order byte
	movl	RetVal, TempD1		
	bsr	getcsum			| get low order byte
	asll	#8, TempD1 		
	orw	TempD1, RetVal		| merge together
	rts

|-------------------------------------------------------------------------------
|  getlsum  --  get a longword and add to checksum
|-------------------------------------------------------------------------------

getlsum:
	clrl	TempD2
	bsr	getwsum			| get high order word 
	movl	RetVal, TempD2
	bsr	getwsum			| get low order word
	swap	TempD2
	orl	TempD2, RetVal		| combine the two
	rts
