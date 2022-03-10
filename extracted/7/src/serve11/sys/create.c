/* create.c - create, newpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <mem.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL create(procaddr,ssize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	int	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	int	pid;			/* stores new process id	*/
	struct	pentry	*pptr;		/* pointer to proc. table entry */
	int	i;
	int	*a;			/* points to list of args	*/
	int	*saddr;			/* stack address		*/
	char	ps;			/* saved processor status	*/
	int	INITRET();
	disable(ps);
	ssize = roundew(ssize);
	if ( ssize < MINSTK || ((saddr=getstk(ssize)) == SYSERR ) ||
		(pid=newpid()) == SYSERR || isodd(procaddr) ||
		priority < 1 ) {
		restore(ps);
		return(SYSERR);
	}
	numproc++;
	pptr = &proctab[pid];
	pptr->pstate = PRSUSP;
	for (i=0 ; i<PNMLEN && (pptr->pname[i]=name[i])!=0 ; i++)
		;
	pptr->pprio = priority;
	pptr->pbase = (short)saddr;
	pptr->pstklen = ssize;
	pptr->psem = 0;
	pptr->phasmsg = FALSE;
	pptr->plimit = (short)((unsigned)saddr - ssize + sizeof(int));
	*saddr-- = MAGIC;
	pptr->pargs = nargs;
	for (i=0 ; i<PNREGS ; i++)
		pptr->pregs[i]=INITREG;
	pptr->pregs[PC] = pptr->paddr = (short)procaddr;
	pptr->pregs[PS] = INITPS;
	pptr->pnxtkin = BADPID;
	pptr->pdevs[0] = pptr->pdevs[1] = BADDEV;
	a = (&args) + (nargs-1);	/* point to last argument	*/
	for ( ; nargs > 0 ; nargs--)	/* machine dependent; copy args	*/
		*saddr-- = *a--;	/* onto created process' stack	*/
	*saddr = (int)INITRET;		/* push on return address	*/
	pptr->pregs[SP] = (int)saddr;
	restore(ps);
	return(pid);
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
