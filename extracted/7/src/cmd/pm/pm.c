#include "pm.h"

/*
 *	pm - a post mortem analyzer of core11 files
 *	that come from the Xinu operating system.
 *
 *	Chris Kent - October 1981, lots of changes
 *	by D. Comer & Droms 82-86
 */

/*
 *	Arguments/options:
 *
 *		pm [-t] [-p] [-s] [ objfile [ corefile ] ]
 *
 *	where objfile is the text that was downloaded, and corefile
 *	is the result of upload. Defaults are a.out and core11.
 *
 */

#define		INTVECTOR	01000	/* # bytes of interrupt vectors	*/

char		*corefile;
FILE		*corefd;	/* the core image			 */
char		*txtfile;
FILE		*txtfd;		/* the system				 */
short		*text;		/* pointer to system image		 */
short		*core;		/* pointer to core image		 */
struct nlist	*symtab;	/* symbol table space 			 */
struct nlist	*esymtab;	/* end of same				 */
struct nlist	*ssymtab;	/* sorted external symbol table space	 */
struct nlist	*essymtab;	/* end of same				 */
struct core11	c_header;	/* registers at time of dump		 */
struct exec	a_out;		/* a.out header				 */
int		errflg;		/* global error indicator		 */
int		allopts, popt, sopt, topt;	/* option indicators	 */
int		verbose;	/* print verbose description or not	 */
char   *vers;			/* pointer to version string		 */
long   clktim;			/* pointer to time variable		 */
struct pentry	*proctb;	/* pointer to image of proc table	 */
struct qent	*Q;		/*    "     "   "   "  q		 */
struct sentry	*semtab;	/*    "     "   "   "  semaphore table	 */
struct tty	*tptr;		/*    "     "   "   "  tty structures	 */
short  currpid;			/* current process id number		 */
main(argc, argv)
int	argc;
char	*argv[];
{

	int		FirstDiff, FirstIDiff;
	register INT	*TextPtr, *CorPtr;
	INT		base;
	char		*pidname;
	char		datestr[80];

	register	i, j;

	setup(argc, argv);	/* open files, etc. */

	/* 
	 * compare the core image text segment and the 
	 * original text segment from the system
	 */
			
	printf("Postmortum for Xinu version %s\n", vers);
	if (clktim > SECPERHR) {
		ascdate(ut2ltim(clktim), datestr);
		printf("Clock was stopped on %s\n",
			datestr);
	}
	FirstDiff = FirstIDiff = 0;
	TextPtr = text;
	CorPtr = core;
	while((char *)CorPtr - (char *)core < INTVECTOR) {
		if(*( (int *)CorPtr) != *( (int *)TextPtr) ) {
			register s, c;

			if(!FirstIDiff){
				printf("Interrupt vectors changed:\n");
				printf("\t\tProgram\t\tCore\n");
				FirstIDiff = 1;
			}
			s = *( (int *)TextPtr);
			c = *( (int *)CorPtr);
			printf("%07o:\t%07o/%04o\t%07o/%04o\n",
			   ((char *)CorPtr - (char *)core),
			   s&0xffff, (s>>16)&0xffff,
			   c&0xffff, (c>>16)&0xffff);
		}
		TextPtr += 2;			/* move past pc & ps	*/
		CorPtr += 2;
	}

	while((char *)CorPtr - (char *)core < a_out.a_text){
		if(*CorPtr != *TextPtr){
			register s, c;

			if(!FirstDiff){
				printf("Text has been changed:\n");
				printf("\t\tProgram\tCore\n");
				FirstDiff = 1;
			}
			s = (int) *TextPtr;
			c = (int) *CorPtr;
			printf("%07o:\t%07o\t%07o\n",
			  ((char *)CorPtr - (char *)core), s&0xffff, c&0xffff);
			/* would be nice to do opcode lookup here */
		}
		TextPtr++;CorPtr++;
	}
	
	/*
	 *	current process info
	 */

	if (currpid<0 || currpid>NPROC) {
		printf("Invalid current process id (currpid==%d)\n",
			currpid);
	} else {
		pidname = proctb[currpid].pname;
		printf("process # %d ( %s ) was running\n", currpid, pidname);
		printf("\tat priority %d ", proctb[currpid].pprio);
		if (	((unsigned)c_header.c_regs[SP]) <
			((unsigned)proctb[currpid].plimit) )
			printf("Stack has OVERFLOWED actual=0%o, limit=0%o\n",
				((unsigned)c_header.c_regs[SP]),
				((unsigned)proctb[currpid].plimit) );
		else
			printf("stack is currently %d words long\n", 
			(proctb[currpid].pbase-c_header.c_regs[SP])/2);
		printf("\tstack range: %0o to %0o\n",
			0177777&proctb[currpid].plimit,
			0177777&proctb[currpid].pbase);
		printregs(c_header.c_regs);
		printf("traceback:\n\n");
		printtrace(c_header.c_regs, proctb[currpid].pbase, currpid);
	}

	if(allopts || popt)
		procdump(currpid);

	if(allopts || sopt)
		semdump();

	if(allopts || topt)
		ttydump();
}

/*
 *	dump the proc table
 */

procdump(currpid)
{

	char		*pidname;
	register int	i;


	for(i = 0; i < NPROC; i++){
		if(i == currpid)
			continue;
		pidname = proctb[i].pname;
		printf("\nprocess # %d ( %s ):\tstate=", i, pidname);
		switch(proctb[i].pstate){
		case PRFREE:
			printf("free\n");
			continue;
		case PRREADY:
			printf("ready\n");
			break;
		case PRCURR:	/* shouldn't happen! */
			printf("CURRENTLY RUNNING???\n");
			break;
		case PRSUSP:
			printf("suspended\n");
			break;
		case PRWAIT:
			printf("waiting on semaphore %d\n", proctb[i].psem);
			break;
		case PRSLEEP:
			printf("sleeping\n");
			break;
		case PRRECV:
			printf("receiving\n");
			break;
		case PRTRECV:
			printf("timing recv.\n");
			break;
		default:
			printf("garbage state 0%o\n", proctb[i].pstate&0xff);
		}
		if (proctb[i].pstate == PRFREE)
			continue;
		printf("\tat priority %d ", proctb[i].pprio);
		printf("stack is %d words long\n", 
			(proctb[i].pbase - proctb[i].pregs[SP])/2);
		printf("\tstack range: %0o to %0o\n",
			0177777&proctb[i].plimit,
			0177777&proctb[i].pbase);
		printregs(proctb[i].pregs);
		printf("traceback:\n\n");
		printtrace(proctb[i].pregs, proctb[i].pbase, i);
		printf("\n");
	}
}

/*
 *	dump the semaphore table and queues
 */

semdump()
{
	short		qptr;
	register int	i, j;
	
	printf("\nSemaphores:\n\n");
	for(i = 0; i < NSEM; i++){
		if (semtab[i].sstate == SFREE)
			continue;
		printf("semaphore # %d : ", i);
		if (semtab[i].sstate != SUSED) {
			printf("garbage state 0%o\n",
				semtab[i].sstate&0xff);
			continue;
		}
		printf("count is %d\n", semtab[i].semcnt);
		if(Q[semtab[i].sqhead].qnext < NPROC) {
			printf("\tqueued proc(s) are: ");
			qptr = semtab[i].sqhead; 
			j = 0;
			do {
				printf("%d ", Q[qptr].qnext);
				qptr = Q[qptr].qnext;
				j++;
			} while(Q[qptr].qnext < NPROC && j<=NPROC );
			if (Q[qptr].qnext != semtab[i].sqtail)
				printf("...QUEUE MALFORMED");
			printf("\n\t%d proc%s actually on queue\n", j,
				j!=1?"s":"");
		}
	}
}

/*
 * routines to print contents of Xinu IO control block
 */
char	*unctrl[]	= {	/* unctrl codes for ttys		*/
	"^@", "^A", "^B", "^C", "^D", "^E", "^F", "^G", "^H", "^I", "^J", "^K",
	"^L", "^M", "^N", "^O", "^P", "^Q", "^R", "^S", "^T", "^U", "^V", "^W",
	"^X", "^Y", "^Z", "^[", "^\\", "^]", "^~", "^_",
	" ", "!", "\"", "#", "$",  "%", "&", "'", "(", ")", "*", "+", ",", "-",
	".", "/", "0",  "1", "2",  "3", "4", "5", "6", "7", "8", "9", ":", ";",
	"<", "=", ">",  "?", "@",  "A", "B", "C", "D", "E", "F", "G", "H", "I",
	"J", "K", "L",  "M", "N",  "O", "P", "Q", "R", "S", "T", "U", "V", "W",
	"X", "Y", "Z",  "[", "\\", "]", "^", "_", "`", "a", "b", "c", "d", "e",
	"f", "g", "h",  "i", "j",  "k", "l", "m", "n", "o", "p", "q", "r", "s",
	"t", "u", "v",  "w", "x",  "y", "z", "{", "|", "}", "~", "^?"
};
ttydump()
{
	register int i;

	for (i=0 ; i<Ntty ; i++)
		tty1dmp(i);
}
tty1dmp(ttynum)
int ttynum;
{
	int len;
	int i;
	struct tty *tp;

	tp = (struct tty *) (
		(unsigned)(tptr) +
		(unsigned)(ttynum* (sizeof(struct tty) - 2) ) );
		/* adjusted for VAX sizes */
	printf("\nTty device %d CSR at 0%o\n",
		ttynum, (*( (short *)(( (char *)(&tp->ifullc))+1))) &0xffff);
		/* actually tp->ioaddr, but Vax aligns differently */
        printf("INPUT -- tail at %d head at %d sem # %d value %d\n",
		tp->itail,tp->ihead,tp->isem,i = semtab[tp->isem].semcnt);
	if(i < 0)
		i = 0;
	else if(i > IBUFLEN)
		i = IBUFLEN;
	ioqdump(tp->itail,i,IBUFLEN,tp->ibuff);
        printf("OUTPUT -- tail at %d head at %d sem # %d value %d\n",
		tp->otail,tp->ohead,tp->osem,i = semtab[tp->osem].semcnt);
	i = OBUFLEN - i;
	if(i < 0)
		i = 0;
	else if(i > OBUFLEN)
		i = OBUFLEN;
	ioqdump(tp->otail,i,OBUFLEN,tp->obuff);
}

ioqdump(start,len,maxlen,buff)
short start,len,maxlen;
char *buff;
{
	register int i;

        for(i=start; len>0; len--){
                if(buff[i]&0200)printf("M-");
                printf("%s", unctrl[buff[i]&0177]);
		if(++i >= maxlen)
			i = 0;
        }
	printf("\n");
}
