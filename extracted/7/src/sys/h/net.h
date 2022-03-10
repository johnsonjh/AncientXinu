/* net.h */

/* High-level network definitions and constants */

#define	NETBUFS		5		/* number of network buffers	*/
#ifndef	Ndg
#define	NETQS		1		/* number of xinu ports used to	*/
#else					/*  demultiplex udp datagrams	*/
#define	NETQS		Ndg
#endif
#define	NETQLEN		3		/* size of a demux queue	*/
#define	NETFQ		3		/* size of input-to-output queue*/
#ifndef	NETNLEN
#define	NETNLEN		30		/* length of network name	*/
#endif

/* Commands for the network pseudo device control call */

#define	NC_SETGW	1		/* set gateway IP address	*/

/* Network input and output processes: procedure name and parameters */

#define	NETIN		netin		/* network input daemon process	*/
#define	NETOUT		netout		/* network output process	*/
extern	int		NETIN(), NETOUT();
#define	NETISTK		350		/* stack size for network input	*/
#define	NETOSTK		400		/* stack size for network output*/
#define	NETIPRI		100		/* network runs at high priority*/
#define	NETOPRI		 99		/* network output priority	*/
#define	NETINAM		"netin"		/* name of network input process*/
#define	NETONAM		"netout"	/* name of network output   "	*/
#define	NETIARGC	1		/* count of args to net input	*/
#define	NETOARGC	2		/* count of args to net output	*/

/* Mapping of external network UDP "port" to internal Xinu port */

struct	netq	{			/* UDP demultiplexing info	*/
	Bool	valid;			/* is this entry in use?	*/
	short	uport;			/* local UDP "port" number	*/
	int	pid;			/* pid of process if one waiting*/
	int	xport;			/* Corresponding Xinu port on	*/
};					/*  which incoming pac. queued	*/

struct	netinfo	{			/* info and parms. for network	*/
	int	netpool;		/* network packet buffer pool	*/
	struct	netq netqs[NETQS];	/* UDP packet demux info	*/
	Bool	mnvalid;		/* is my host name field valid?	*/
	Bool	mavalid;		/* is my network address valid?	*/
	char	myname[NETNLEN];	/* domain name of this machine	*/
	IPaddr	myaddr;			/* IP address of this machine	*/
	IPaddr	mynet;			/* Network portion of myaddr	*/
	IPaddr	gateway;		/* IP address of gateway to use	*/
	int	nxtprt;			/* next available local UDP port*/
	int	nmutex;			/* output mutual excl. semaphore*/
	int	npacket;		/* # of packets processed	*/
	int	ndrop;			/* # of packets discarded	*/
	int	nover;			/* # dropped because queue full	*/
};

extern	struct	netinfo	Net;		/* All network parameters	*/
