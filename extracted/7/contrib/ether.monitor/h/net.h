/* net.h */

/* High-level network definitions and constants */

#define	NETBUFS		5		/* number of network buffers	*/
#ifndef	Ndg
#define	NETQS		1		/* number of xinu ports used to	*/
#else					/*  demultiplex udp datagrams	*/
#define	NETQS		Ndg
#endif
#define	NETQLEN		3		/* size of a demux queue	*/

/* Commands for the network pseudo device control call */

#define	NC_SETGW	1		/* set gateway IP address	*/

/* Network input process procedure name and process parameters */

#define	NETPROC		network		/* network input daemon process	*/
extern	int		NETPROC();
#define	NETSTK		350		/* stack size for network proc.	*/
#define	NETSSTK		500		/* stack size for network start	*/
#define	NETPRIO		100		/* network runs at high priority*/
#define	NETNAME		"network"	/* name of network input process*/
#define	NETARGS		0		/* count/arguments for net proc	*/
#define	NETSTART	netstart	/* net output startup process	*/
extern	int		NETSTART();
#define	NETSNAM		"netstart"	/* name of output start process	*/

/* Important network IP addresses in "dot" form */

#define	NETGATE		192,5,48,2	/* gateway IP address		*/
#define	NETSERVE	192,5,48,3	/* address of my server		*/

/* Mapping of external network UDP "port" to internal Xinu port */

struct	netq	{			/* UDP demultiplexing info	*/
	Bool	valid;			/* if this entry in use?	*/
	short	uport;			/* local UDP "port" number	*/
	int	pid;			/* pid of process if one waiting*/
	int	xport;			/* Corresponding Xinu port on	*/
};					/*  which incoming pac. queued	*/

struct	netinfo	{			/* info and parms. for network	*/
	int	netpool;		/* network packet buffer pool	*/
	struct	netq netqs[NETQS];	/* UDP packet demux info	*/
	Bool	mavalid;		/* is my network address valid?	*/
	IPaddr	myaddr;			/* IP address of this machine	*/
	IPaddr	gateway;		/*  "  "  of gateway		*/
	int	nxtprt;			/* next available local UDP port*/
	int	nmutex;			/* output mutal excl. semaphore	*/
	int	npacket;		/* number of packets processed	*/
	int	ndrop;			/* number of dropped packets	*/
	int	nover;			/* # dropped because queue full	*/
};

extern  struct netinfo Net;                     /* All network parameters       */
