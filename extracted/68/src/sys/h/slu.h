/* slu.h */

/* standard serial line unit device constants */

#define	SLUENABLE	0100		/* device interrupt enable bit	*/
#define	SLUREADY	0200		/* device ready bit		*/
#define	SLUDISABLE	0000		/* device interrupt disable mask*/
#define	SLUTBREAK	0001		/* transmitter break-mode bit	*/
#define	SLUERMASK	0170000		/* mask for error flags on input*/
#define	SLUCHMASK	0377		/* mask for input character	*/

/* SLU device register layout and correspondence to vendor's names	*/

struct	csr	{
	int	crstat;		/* receiver control and status	(RCSR)	*/
	int	crbuf;		/* receiver data buffer		(RBUF)	*/
	int	ctstat;		/* transmitter control & status (XCSR)	*/
	int	ctbuf;		/* transmitter data buffer	(XBUF)	*/
};

/* In-line SLU commands							*/

#define	realcsr(devptr)		(struct csr *) (devptr->dvcsr)
#define slusetbrk(ioaddr)	(ioaddr)->ctstat |= SLUTBREAK
#define slurstbrk(ioaddr)	(ioaddr)->ctstat &= ~SLUTBREAK
#define slugetch(ioaddr)	(ioaddr)->crbuf
#define slugetcher(ioaddr)	(ioaddr)->crbuf
#define sluputch(ioaddr, ch)	(ioaddr)->ctbuf = (ch)
#define slurenable(ioaddr, x)	(ioaddr)->crstat |= SLUENABLE
#define slurdisable(ioaddr, x)	(ioaddr)->crstat &= ~SLUENABLE
#define slutenable(ioaddr, x)	(ioaddr)->ctstat |= SLUENABLE
#define slutdisable(ioaddr, x)	(ioaddr)->ctstat &= ~SLUENABLE
