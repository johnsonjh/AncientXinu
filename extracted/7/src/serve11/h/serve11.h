/* serve11.h */

/* Definitions for standalone server machine */

#define	MYNAME	"serve11"	/* default name of this machine */

struct	serv	{		/* all information about servers	*/
	Bool	ser_rarp;	/* Do WE provide RARP server?		*/
	Bool	ser_time;	/* Do WE provide time server?		*/
	Bool	ser_name;	/* Do WE provide name server?		*/
	Bool	ser_file;	/* Do WE provide (pseudo) file server?	*/
	IPaddr	ser_ip;		/* If ser_rarp, use this net IP address	*/
	int	ser_rcnt;	/* count of calls to our RARP server	*/
	int	ser_tcnt;	/* count of calls to our time server	*/
	int	ser_ncnt;	/* count of calls to our name server	*/
	int	ser_fcnt;	/* count of calls to our file server	*/
};

extern	struct	serv	serinfo;
extern	struct	mnament	mnametab[MNTABSIZE];
extern	int		mntabsize;
