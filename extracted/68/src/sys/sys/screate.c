/* screate.c - screate, newsem */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>

/*------------------------------------------------------------------------
 * screate  --  create and initialize a semaphore, returning its id
 *------------------------------------------------------------------------
 */
SYSCALL screate(count)
	int	count;			/* initial count (>=0)		*/
{
	int	sem;

#ifdef	DEBUG
	dotrace("screate",&count, 1);
#endif
	disable();
	if ( count<0 || (sem=newsem())==SYSERR ) {
		restore();
		return(SYSERR);
	}
	semaph[sem].semcnt = count;
	/* sqhead and sqtail were initialized at system startup */
	restore();
	return(sem);
}

/*------------------------------------------------------------------------
 * newsem  --  allocate an unused semaphore and return its index
 *------------------------------------------------------------------------
 */
LOCAL	newsem()
{
	int	sem;
	int	i;

#ifdef	DEBUG
	dotrace("newsem", NULL, 0);
#endif
	for (i=0 ; i<NSEM ; i++) {
		sem=nextsem--;
		if (nextsem < 0)
			nextsem = NSEM-1;
		if (semaph[sem].sstate==SFREE) {
			semaph[sem].sstate = SUSED;
			return(sem);
		}
	}
	return(SYSERR);
}
