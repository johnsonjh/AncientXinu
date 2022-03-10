struct	exec {	
	short		a_magic;	
	unsigned short	a_text;		
	unsigned short	a_data;		
	unsigned short	a_bss;		
	unsigned short	a_syms;		
	unsigned short	a_entry;	
	unsigned short	a_unused;	
	char		a_flag;		
	char		a_v6;		
};
#define	A_MAGIC1	0407       	
#define	A_MAGIC2	0410       	
#define	A_MAGIC3	0411       	
#define	A_MAGIC4	0405       	
struct	nlist {				
	char		n_name[8];	
	short		n_type;		
	unsigned short	n_value;	
};
#define	N_UNDF		0		
#define	N_ABS		01		
#define	N_TEXT		02		
#define	N_DATA		03		
#define	N_BSS		04		
#define	N_TYPE		037
#define	N_REG		024		
#define	N_FN		037		
#define	N_EXT		040		
#define	FORMAT		"%06o"		
#define	NULLPTR	(char *)0
struct	devsw	{			
	short	dvnum;
	char	*dvname;
	short	(*dvinit)();
	short	(*dvopen)();
	short	(*dvclose)();
	short	(*dvread)();
	short	(*dvwrite)();
	short	(*dvseek)();
	short	(*dvgetc)();
	short	(*dvputc)();
	short	(*dvcntl)();
	short	dvcsr;
	short	dvivec;
	short	dvovec;
	short	(*dviint)();
	short	(*dvoint)();
	char	*dvioblk;
	short	dvminor;
	};
extern	struct	devsw devtab[];		
#define	CONSOLE     0			
#define	ETHER       1			
#define	INTERNET    2			
#define	DGRAM1      3			
#define	DGRAM2      4			
#define	DGRAM3      5			
#define	DGRAM4      6			
#define	RFILSYS     7			
#define	RFILE1      8			
#define	RFILE2      9			
#define	RFILE3      10			
#define	RFILE4      11			
#define	NAMESPACE   12			
#define	Ntty	1
#define	Neth	1
#define	Ndgm	1
#define	Ndg	4
#define	Nrfm	1
#define	Nrf	4
#define	Nnam	1
#define	NDEVS	13
extern	short	ttyinit();
extern	short	ttyopen();
extern	short	ionull();
extern	short	ttyread();
extern	short	ttywrite();
extern	short	ioerr();
extern	short	ttycntl();
extern	short	ttygetc();
extern	short	ttyputc();
extern	short	ttyiin();
extern	short	ttyoin();
extern	short	ethinit();
extern	short	ethread();
extern	short	ethwrite();
extern	short	ethinter();
extern	short	dgmopen();
extern	short	dgmcntl();
extern	short	dginit();
extern	short	dgclose();
extern	short	dgread();
extern	short	dgwrite();
extern	short	dgcntl();
extern	short	rfopen();
extern	short	rfcntl();
extern	short	rfinit();
extern	short	rfclose();
extern	short	rfread();
extern	short	rfwrite();
extern	short	rfseek();
extern	short	rfgetc();
extern	short	rfputc();
extern	short	naminit();
extern	short	namopen();
#define	NPROC	    10			
#define	NSEM	    30			
#define	MEMMARK				
#define	RTCLOCK				
#define	STKCHK				
#define	NETDAEMON			
#define	GATEWAY	     128,10,2,70	
#define	TSERVER	    "128.10.2.3:37"	
#define	RSERVER	    "128.10.2.3:2001"	
#define	NSERVER	    "128.10.2.3:53"	
#define	VERSION	    "7.6 (01/11/87)"	
#define	REGISTERS 	8		
#define	NREGS		REGISTERS+1	
#define	COREMAGIC	0477		
struct	core11 {			
	short c_magic;			
	short c_size;			
	short c_zero1;			
	short c_zero2;			
	short c_regs[REGISTERS];	
	short c_psw;			
	short c_zero3;			
	short c_zero4;			
	short c_zero5;			
};
#define	R0	0
#define	R1	1
#define	R2	2
#define	R3	3
#define	R4	4
#define	R5	5
#define	R6	6
#define	R7	7
typedef	char		Bool;		
#define	FALSE		0		
#define	TRUE		1
#define	NULLCH		'\0'		
#define	NULLSTR		""		
#define	SYSCALL		short		
#define	LOCAL		static		
#define	COMMAND		short		
#define	BUILTIN		short		
#define	INTPROC		short		
#define	PROCESS		short		
#define	RESCHYES	1		
#define	RESCHNO		0		
#define	MININT		0100000		
#define	MAXINT		0077777		
#define	LOWBYTE		0377		
#define	HIBYTE		0177400		
#define	LOW16		0177777		
#define	SP		6		
#define	PC		7		
#define	PS		8		
#define	MINSTK		40		
#define	NULLSTK		300		
#define	DISABLE		0340		
#define	MAGIC		0125252		
#define	OK		 1		
#define	SYSERR		-1		
#define	TIMEOUT		-3		
#define	INTRMSG		-4		
#define	INITSTK		500		
#define	INITPRIO	20		
#define	INITNAME	"main"		
#define	INITARGS	1,0		
#define	INITRET		userret		
#define	INITPS		0		
#define	INITREG		0		
#define	QUANTUM		10		
#define	isodd(x)	(01&(short)(x))
#define	min(a,b)	( (a) < (b) ? (a) : (b) )
#define	disable(ps)	asm("mfps ~ps");asm("mtps $0340")
#define	restore(ps)	asm("mtps ~ps")	
#define	enable()	asm("mtps $000")
#define	pause()		asm("wait")	
#define	halt()		asm("halt")	
extern	short	rdyhead, rdytail;
extern	short	preempt;
#ifndef	NQENT
#define	NQENT		NPROC + NSEM + NSEM + 4	
#endif
struct	qent	{		
	short	qkey;		
	short	qnext;		
	short	qprev;		
	};
extern	struct	qent q[];
extern	short	nextqueue;
#define	isempty(list)	(q[(list)].qnext >= NPROC)
#define	nonempty(list)	(q[(list)].qnext < NPROC)
#define	firstkey(list)	(q[q[(list)].qnext].qkey)
#define lastkey(tail)	(q[q[(tail)].qprev].qkey)
#define firstid(list)	(q[(list)].qnext)
#define	EMPTY	-1		
#ifndef	NPROC				
#define	NPROC		10		
#endif
#define	PRCURR		'\001'		
#define	PRFREE		'\002'		
#define	PRREADY		'\003'		
#define	PRRECV		'\004'		
#define	PRSLEEP		'\005'		
#define	PRSUSP		'\006'		
#define	PRWAIT		'\007'		
#define	PRTRECV		'\010'		
#define	PNREGS		9		
#define	PNMLEN		8		
#define	NULLPROC	0		
#define	BADPID		-1		
#define	isbadpid(x)	(x<=0 || x>=NPROC)
struct	pentry	{
	char	pstate;			
	short	pprio;			
	short	pregs[PNREGS];		
	short	psem;			
	short	pmsg;			
	Bool	phasmsg;		
	short	pbase;			
	short	pstklen;		
	short	plimit;			
	char	pname[PNMLEN];		
	short	pargs;			
	short	paddr;			
	short	pnxtkin;		
	short	pdevs[2];		
};
extern	struct	pentry proctab[];
extern	short	numproc;		
extern	short	nextproc;		
extern	short	currpid;		
#ifndef	NSEM
#define	NSEM		35	
#endif
#define	SFREE	'\01'		
#define	SUSED	'\02'		
struct	sentry	{		
	char	sstate;		
	short	semcnt;		
	short	sqhead;		
	short	sqtail;		
};
extern	struct	sentry	semaph[];
extern	short	nextsem;
#define	isbadsem(s)	(s<0 || s>=NSEM)
#define	OBMINSP		20		
#define	EBUFLEN		20		
#ifndef	Ntty
#define	Ntty		1		
#endif
#ifndef	IBUFLEN
#define	IBUFLEN		128		
#endif
#ifndef	OBUFLEN
#define	OBUFLEN		64		
#endif
#define	IMRAW		'R'		
#define	IMCOOKED	'C'		
#define	IMCBREAK	'K'		
#define	OMRAW		'R'		
struct	tty	{			
	short	ihead;			
	short	itail;			
	char	ibuff[IBUFLEN];		
	short	isem;			
	short	ohead;			
	short	otail;			
	char	obuff[OBUFLEN];		
	short	osem;			
	short	odsend;			
	short	ehead;			
	short	etail;			
	char	ebuff[EBUFLEN];		
	char	imode;			
	Bool	iecho;			
	Bool	ieback;			
	Bool	evis;			
	Bool	ecrlf;			
	Bool	icrlf;			
	Bool	ierase;			
	char	ierasec;		
	Bool	ikill;			
	char	ikillc;			
	Bool	iintr;			
	char	iintrc;			
	short	iintpid;		
	Bool	ieof;			
	char	ieofc;			
	short	icursor;		
	Bool	oflow;			
	Bool	oheld;			
	char	ostop;			
	char	ostart;			
	Bool	ocrlf;			
	char	ifullc;			
	struct	csr	*ioaddr;	
};
extern	struct	tty tty[];
#define	BACKSP	'\b'			
#define	BELL	'\007'			
#define	BLANK	' '			
#define	KILLCH	'\025'			
#define	NEWLINE	'\n'			
#define	RETURN	'\r'			
#define	STOPCH	'\023'			
#define	STRTCH	'\021'			
#define	INTRCH	'\002'			
#define	UPARROW	'^'			
#define	TCSETBRK	1		
#define	TCRSTBRK	2		
#define	TCNEXTC		3		
#define	TCMODER		4		
#define	TCMODEC		5		
#define	TCMODEK		6		
#define	TCICHARS	8		
#define	TCECHO		9		
#define	TCNOECHO	10		
#define	TCINT		11		
#define	TCINTCH		12		
#define	TCNOINT		13		
#define	TFULLC		BELL		
#define	net2xt(x)	((x)-2208988800L)  
#define	xt2net(x)	((x)+2208988800L)  
struct	datinfo	{
	short	dt_msize[12];
	char	*dt_mnam[12];
};
extern	struct	datinfo	Dat;
#define	isleap(x)	((x)%4==0)	
#define	SECPERDY	(60L*60L*24L)	
#define	SECPERHR	(60L*60L)	
#define	SECPERMN	(60L)		
#define	ZONE_EST	5		
#define	ZONE_CST	6		
#define	ZONE_MST	7
#define	ZONE_PST	8
#define	TIMEZONE	ZONE_EST	
#define	ut2ltim(x)	((x)-TIMEZONE*SECPERHR)
#define	ltim2ut(x)	((x)+TIMEZONE*SECPERHR)
#ifndef	TSERVER
#define	TSERVER		"128.10.2.3:37"
#endif
