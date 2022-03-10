halt=0
reset=5
/
/ Downloader - two part download program for LSI-11/02
/
/     Port constants
/
RCSR=0		/ Register displacements
RDATA=2		/ .
TCSR=4		/ .
TDATA=6		/ .
P0=177560
/
PACKSIZE=200    / maximum packet size
SOH='/		/ start-of-header
ACK='y
NAK='n
ESC=33
/
/
/ Phase 0
/
/ this gets loaded at location zero - loading stops with the 177777.
/ It reads the downloader from the high speed line into the topmost
/ part of memory and then halts.
/
/   Arguments:
/	r1 - length of bss section (this is zeroed)
/	r2 - time delay in seconds before starting program (<0 then halt)
/	r3 - starting address of the program if r2 >= 0
/	r4 - base address of RCSR
/
/   Stack frame displacements
/
bsslen=-2
delay=-4
pstart=-6		/ program start address
sum=-10			/ global checksum
packbuf=-12		/ address of packet buffer
baseaddr=-14		/ address field of current packet
length=-16		/ length field from current packet
FRAMELEN=200            /   frame length


start:
        reset
        jbr begin               / skip exception vector
	excpt; 340
begin:
0:	bitb	$200,*$P0+RCSR	/ wait for character
	beq	0b
excpt:
	mov	$stack,sp	/ reload stack pointer
	movb	*$P0+RDATA,-(r0) / try storing, if fail, dec and retry
	sob	r1,0b		/ success - load remaining bytes
	mov	r0,sp		/ set sp to beginning of program
	halt			/ return control to ODT, VAX issues "P"
	jmp	(sp)		/ start program
	177777
stack=.+4
/
/  Phase 1
/
/  This program is copied into the topmost portion of memory by phase 0.
/  It must be relocatable (no PC-relative modes).
/
phase1:
	mov	sp,r5		/ allocate a stack frame
        sub     $PACKSIZE*2+FRAMELEN,sp
	mov	r1,bsslen(r5)	/ save arguments
	mov	r2,delay(r5)
	mov	r3,pstart(r5)
	mov	sp,packbuf(r5)
retry:
0:	jsr	pc,rawgetc	/ scan for SOH
	cmpb	$SOH,r0
	bne	0b
	clr	sum(r5)		/ start check summing
	jsr	pc,getwsum	/ get base address
	mov	r0,baseaddr(r5)
	jsr	pc,getwsum	/ get packet length
	mov	r0,length(r5)
	cmp	$PACKSIZE,r0	/ valid?
	blt	sendnak		/ no - send nak
/
/  read data words into the packet buffer on the stack
/
	mov	r0,r3		/ set up registers for loop
        beq     getsum
	mov	packbuf(r5),r2
1:	jsr	pc,getwsum	/ get a data word
	mov	r0,(r2)+	/ save in buffer on stack
	sob	r3,1b
/
/  read and check checksum
/
getsum:
	jsr	pc,getwsum
	tst	sum(r5)		/ sum should now be exactly zero
	beq	copypack	/  yes=>copy packet into place
/
/ invalid packet - send NAK and retry
/
sendnak:
	mov	$NAK,r0
        jsr     pc,rawputc
	jbr	retry
/
/ valid packet - copy into place and send the ACK
/
copypack:
	mov	baseaddr(r5),r2
	mov	packbuf(r5),r1
	mov	length(r5),r3
	beq	sendack
0:	mov	(r1)+,(r2)+
	sob	r3,0b
sendack:
	mov	$ACK,r0
        jsr     pc,rawputc
/
/ if the length was zero then that's all there is
/
	tst	length(r5)
	bne	retry
/
/ zero out bss - if there is any
/
	mov	bsslen(r5),r1
	beq	1f
	mov	baseaddr(r5),r0
0:	clrb	(r0)+
	sob	r1,0b
1:
/
/ implement auto start options - derived from Chris Kent's code
/
	mov	delay(r5),r0	/ get delay
	tst	r0		/ if <0, don't start loaded program
	bgt	1f
	beq	3f		/ if = 0, just start it
	halt
1:
	asl	r0		/ multiply by 2
1:
	mov	$175000,r1	/ .5 sec time constant
2:
	dec	r1
	bhi	2b		/ .5 sec loop end
	dec	r0
	bhi	1b		/ timer loop end
3:
	jmp	*pstart(r5)	/ start the program
/
/ getwsum - get a word and add into the checksum
/ getw - get a word from the line
/
getwsum:
	jsr	pc,getw		/ get a word
	add	r0,sum(r5)
	rts	pc
getw:
	jsr	pc,getc		/ get low order byte
	mov	r0,r1		/ save.
	jsr	pc,getc		/ get high order byte
        swab    r0
        bis     r1,r0           / merge together
	rts	pc
/
/ getc - get a character, honoring ESCing
/
getc:
	jsr	pc,rawgetc	/ get a character
	cmpb	$ESC,r0		/ is it an escape
	bne	1f
	jsr	pc,rawgetc	/ get the real one
1:	rts	pc
/
/ rawgetc - get a character from the line
/
rawgetc:
0:	bitb	$200,RCSR(r4)
	beq	0b
	movb	RDATA(r4),r0
        bic     $177400,r0
	rts	pc

/
/ rawputc - put a character on the line
/
rawputc:
0:	bitb	$200,TCSR(r4)
	beq	0b
	movb	r0,TDATA(r4)
	rts	pc
