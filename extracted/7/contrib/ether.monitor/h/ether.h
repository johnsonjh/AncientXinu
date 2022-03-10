/* ether.h */

/* Ethernet definitions and constants */

#define	EMINPAK	64		/* minimum packet length		*/
#define	EMAXPAK	1150		/* 1536. (cut here to conserve space)	*/
#define	EHLEN	14		/* size of Ethernet packet header	*/
#define	EDLEN	EMAXPAK-EHLEN	/* length of data field in ether packet	*/
#define	EPADLEN	6		/* number of octets in physical address	*/
typedef	char	Eaddr[EPADLEN];	/* length of physical address (48 bits)	*/
#define	EXRETRY	3		/* number of times to retry xmit errors	*/
#define	EBCAST	"\377\377\377\377\377\377"/* Ethernet broadcast address	*/

struct	eheader	{		/* format of header in Ethernet packet	*/
	Eaddr	e_dest;		/* destination host address		*/
	Eaddr	e_src;		/* source host address			*/
	short	e_ptype;	/* Ethernet packet type (see below)	*/
};

#define	EP_LOOP	0x0060		/* packet type: Loopback		*/
#define	EP_ECHO	0x0200		/* packet type: Echo			*/
#define	EP_PUP	0x0400		/* packet type: Xerox PUP protocol	*/
#define	EP_IP	0x0800		/* packet type: DARPA Internet protocol	*/
#define	EP_ARP	0x0806		/* packet type: Address resolution  "	*/
#define	EP_RARP	0x8035		/* packet type: reverse  "  "       "	*/

struct	epacket	{		/* complete structure of Ethernet packet*/
	struct	eheader	ep_hdr;	/* packet header			*/
	char	ep_data[EDLEN];	/* data in the packet			*/
};

/* Ethernet control block descriptions */

struct	etblk	{
	struct	dqregs *eioaddr;/* address of device csr		*/
	struct	devsw	*etdev;	/* Address of device switch table entry	*/
	Eaddr	etpaddr;	/* Ethernet physical device address	*/
	int	etrpid;		/* id of process reading from ethernet	*/
	int	etrsem;		/* mutex for reading from the ethernet	*/
	int	etwsem;		/* mutex for writing to the ethernet	*/
	int	etsetup;	/* DC_NORM or DC_SETUP for normal/setup	*/
	int	etlen;		/* lenght of packet being written or 0	*/
	int	etwtry;		/* num. of times to retry xmit errors	*/
	struct	dcmd ercmd[2];	/* deqna read command descriptor plus	*/
				/*  end-of-list				*/
	struct	dcmd ewcmd[2];	/* deqna write command descriptor plus	*/
				/*  end-of-list				*/
};

extern	struct	etblk	eth[];
