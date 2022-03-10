halt=0
reset=5
/
/ uploader
/
/
/ frame	displacements...
/
base=-2
length=-4
chksum=-6
words=-10
/
/ I/O register displacements
/
P0=177560
RCSR=0
RDATA=2
TCSR=4
TDATA=6
/
/ constants...
/
ACK='y
NAK='n
SOH='/
ESC=33
STOPC=23
STARTC=21
MAXUPL=40

/*
/* boot	loader for uploader...
/*
boot:
        reset
        clr     r0              / this is were we start
0:	bitb	$200,*$P0+RCSR	/ wait for a character
	beq	0b
1:	bitb	$200,*$P0+TCSR	/ wait for transmitter,	too
	beq	1b
	movb	(r0),*$P0+TDATA
	movb	*$P0+RDATA,(r0)+
	sob	r1,0b
	halt
	jbr	upld		/ enter	uploader
	177777

start:

/
/ uploader - sends memory packets to VAX
/
/ Inputs:	r0 - base address at which to start
/		r1 - word count	(not byte count!!)
/		r4 - base address of CSR
/
upld:
	mov	sp,r5
	sub	$10,sp
	mov	r0,base(r5)
	mov	r1,length(r5)
/ compute packet length
pkloop:
	cmp	$MAXUPL,length(r5)	/ enough to fill a packet?
	bge	0f			/   yes, jump
	mov	$MAXUPL,words(r5)
	jbr	1f
0:
	mov	length(r5),words(r5)	/   no,	use remaining length
1:
	ble	nomore			/   No,	exit
	sub	words(r5),length(r5)
/
/ format and send a packet...
/
retry:
	jsr	pc,sendpack
	mov	words(r5),r3
	mov	base(r5),r2
2:
	mov	(r2)+,r0		/ send one word
	jsr	pc,putwsum
	dec	r3
	bgt	2b

	mov	chksum(r5),r0
	jsr	pc,putw
/
/ wait for ack from VAX
/
ackloop:
	jsr	pc,rawgetc
	cmpb	r0,$NAK
	beq	retry
	cmpb	r0,$ACK
	bne	ackloop
	mov	r2,base(r5)
	jbr	pkloop
/
/ end of transmission
/
nomore:
	jsr	pc,sendpack
	mov	chksum(r5),r0
	jsr	pc,putw
	halt
/
/ send a packet	header...
/
sendpack:
	mov	$SOH,r0			/ send start of	header
	jsr	pc,rawputc
	clr	chksum(r5)
	mov	words(r5),r0		/ send word count
	jsr	pc,putwsum
	mov	base(r5),r0		/ send address
	jsr	pc,putwsum
	rts	pc
/
/ character routines...
/
/  These routines assume that r4 contains the base addr	of the CSR
/
/ putw,	putwsum: put word out on line
/
putwsum:
	add	r0,chksum(r5)
putw:
	jsr	pc,putc			/ send low order byte
	swab	r0
	jsr	pc,putc			/ send high order byte
	swab	r0			/   restore r0...
	rts	pc

/
/ putc:	put character with byte	stuffing for SOH and ESC
/
putc:
	cmpb	$SOH,r0
	beq	sendesc
	cmpb	$ESC,r0
	bne	notesc
sendesc:
	mov	r0,-(sp)
	mov	$ESC,r0
	jsr	pc,rawputc
	mov	(sp)+,r0
notesc:
	jsr	pc,rawputc
	rts	pc
/
/ rawputc: send	out r0 byte, no	stuffing, with CTRL-S/Q	handshake
/
rawputc:
	bitb	$200,RCSR(r4)		/ check	for incoming character
	beq	sendc
	mov	RDATA(r4),r1
	cmpb	$STOPC,r1
	bne	sendc
stoploop:
	bitb	$200,RCSR(r4)
	beq	stoploop
	mov	RDATA(r4),r1
	cmpb	$STARTC,r1
	bne	stoploop
sendc:
	bitb	$200,TCSR(r4)
	beq	sendc
	movb	r0,TDATA(r4)
	rts	pc
/
/ rawgetc: get character without esc unstuffing
/
rawgetc:
	bitb	$200,RCSR(r4)		/ wait for receiver full
	beq	rawgetc
	movb	RDATA(r4),r0		/ read character
	rts	pc















/
/ Size up memory.
/   Inputs:  r0: high address at which to start
/
/   Outputs: r0: highest responding address < input r0
/
sizemem:
	mov *$4,-(sp)
	mov *$6,-(sp)
	mov $excpt,*$4
	mov $340,$*6
loop:
	mov (r0),r2
	mov (sp)+,*$6
	mov (sp)+,*$4
	rts pc
excpt:
	add $4,sp
	sub $2,r0
	jbr loop
