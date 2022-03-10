/* conf.h (GENERATED FILE; DO NOT EDIT) */

#define	NULLPTR	(char *)0

/* Device table declarations */
struct	devsw	{			/* device table entry */
	int	dvnum;
	int	(*dvinit)();
	int	(*dvopen)();
	int	(*dvclose)();
	int	(*dvread)();
	int	(*dvwrite)();
	int	(*dvseek)();
	int	(*dvgetc)();
	int	(*dvputc)();
	int	(*dvcntl)();
	int	dvcsr;
	int	dvivec;
	int	dvovec;
	int	(*dviint)();
	int	(*dvoint)();
	char	*dvioblk;
	int	dvminor;
	};

extern	struct	devsw devtab[];		/* one entry per device */


/* Device name definitions */

#define	CONSOLE     0			/* type tty      */
#define	OTHER       1			/* type tty      */
#define	DISK0       2			/* type dsk      */
#define	FILE0       3			/* type df       */
#define	FILE1       4			/* type df       */
#define	FILE2       5			/* type df       */
#define	FILE3       6			/* type df       */

/* Control block sizes */

#define	Ntty	2
#define	Ndsk	1
#define	Ndf	4

#define	NDEVS	7

/* Declarations of I/O routines referenced */

extern	int	ttyinit();
extern	int	ioerr();
extern	int	ttyread();
extern	int	ttywrite();
extern	int	ttycntl();
extern	int	ttygetc();
extern	int	ttyputc();
extern	int	sluint();
extern	int	dsinit();
extern	int	dsopen();
extern	int	dsread();
extern	int	dswrite();
extern	int	dsseek();
extern	int	dscntl();
extern	int	dsinter();
extern	int	lfinit();
extern	int	lfclose();
extern	int	lfread();
extern	int	lfwrite();
extern	int	lfseek();
extern	int	lfgetc();
extern	int	lfputc();


/* Configuration and Size Constants */

#define MEMMARK				/* use memory marking		*/
#define	NPROC	10			/* number of user processes	*/
#define	NSEM	50			/* total number of semaphores	*/
#define	RTCLOCK
#define	VERSION	"3.0 (8/6/83)"		/* label printed at startup	*/
