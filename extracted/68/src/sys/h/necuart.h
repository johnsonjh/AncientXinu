
/* nec 7201 serial line unit device constants */

#define SLUCHMASK	0x00ff
#define SLUERMASK	0xff00

/* Control and Status Register Constants				*/

/* Control Register 0  - Frequently Used Commands and Register Pointer	*/
#define	WR0		0
#define	WR0A		0
#define NECchanres	0x18	/* Channel Reset			*/
#define NECtxires	0x28	/* Transmitter Interrupt Reset		*/
#define NECextres	0x10	/* External/Status Interrupt Reset	*/
#define NECerres	0x30	/* Error Reset				*/
#define NECeoi		0x38	/* End of Interrupt			*/

/* Control Register 1  - Interrupt Control				*/
#define	WR1		1
#define	WR1A		1
#define NECtxiena	0x02	/* Transmitter Interrupt Enable		*/
#define NECrxiena	0x18	/* Receiver Interrupt On All Enable	*/
#define NECcond		0x04	/* Condition Affects Vector		*/

/* Control Register 2  - Status Vector					*/
#define	WR2A		2

/* Control Register 3  - Receiver Control				*/
#define	WR3		3
#define NECrxena	0x01	/* Receiver Enable			*/
#define NECrxauto	0x20	/* Make DCD and CTS enable rx and tx	*/
#define NECrx8bt	0xc0	/* Set Receiver Characters to 8 bits	*/

/* Control Register 4  - Mode Control					*/
#define	WR4		4
#define NECrx1sb	0x04	/* Assume One Receiver Stop Bit		*/
#define NEC9600b	0x40	/* 9600 baud				*/
#define NEC4800b	0x80	/* 4800 baud				*/
#define NEC2400b	0xc0	/* 2400 baud				*/

/* Control Register 5  - Transmitter Control				*/
#define	WR5		5
#define NECtxrts	0x02	/* Ready To Send			*/
#define NECtxena	0x08	/* Tranmitter Enable			*/
#define NECtx8bt	0x60	/* Set Transmitter Characters to 8 bits	*/
#define NECtxdtr	0x80	/* Data Terminal Ready			*/
#define NECtxbreak	0x10	/* Send Break				*/

/* Status  Register 0  - Buffer and External/Status Status		*/
#define	RD0		0
#define	RD0A		0
#define NECrxrdy	0x01	/* Character in Receiver buffer		*/
#define NECtxrdy	0x04	/* Tranmitter Buffer Clear		*/
#define NECbreak	0x80	/* Break Condition Set			*/

/* Status  Register 1  - Received Character Error and Special Condition */
#define	RD1		1
#define	RD1A		1
#define NECparity	0x10	/* Parity Error				*/
#define	NECoverrun	0x20	/* Overrun Error			*/
#define NECframing	0x40	/* Framing Error			*/
#define NECerror	(NECparity|NECoverrun|NECframing)

/* Status  Register 2b - Modified Interrupt Vector			*/
#define NECchan		0x10	/* Channel   TRUE->A   FALSE->B		*/
#define NECtxclear	0x00	/* Transmitter Buffer Empty		*/
#define NECextern	0x04	/* External/Status Change		*/
#define NECreceive	0x08	/* Received Character Available		*/
#define NECspecial	0x0c	/* Special Receive Condition		*/

/* control and status register device structure */

struct csr { 
	char	crtbuf;
	char	crtpad1;
	char	crtstat;
	char	crtpad2;
};

/* in-line control functions			*/

#define	realcsr(devptr)\
	isodd(devptr->dvminor) \
		? (struct csr *) (devptr->dvcsr + 4) \
		: (struct csr *) (devptr->dvcsr)

#define	pokeuart(ioaddr, reg, data)\
	(ioaddr)->crtstat = (reg);\
	(ioaddr)->crtstat = (data)

#define slusetbrk(ioaddr)		\
	pokeuart(ioaddr, WR5, NECtxrts|NECtxdtr|NECtx8bt|NECtxena|NECbreak)

#define	slurstbrk(ioaddr)		\
	pokeuart(ioaddr, WR5, NECtxrts|NECtxdtr|NECtx8bt|NECtxena)

#define sluputch(ioaddr,ch)	\
	(ioaddr)->crtbuf = (ch)

#define	slugetch(ioaddr)	\
	(ioaddr)->crtbuf

#define slutdisable(ioaddr, ttyp)	\
	(ttyp)->intrstate &= ~NECtxiena;\
	pokeuart(ioaddr, WR1, (ttyp)->intrstate)

#define	slutenable(ioaddr, ttyp)	\
	(ttyp)->intrstate |= NECtxiena;\
	pokeuart(ioaddr, WR1, (ttyp)->intrstate)

#define slurdisable(ioaddr, ttyp)	\
	(ttyp)->intrstate &= ~NECrxiena;\
	pokeuart(ioaddr, WR1, (ttyp)->intrstate)

#define	slurenable(ioaddr, ttyp)	\
	(ttyp)->intrstate |= NECrxiena;\
	pokeuart(ioaddr, WR1, (ttyp)->intrstate)
