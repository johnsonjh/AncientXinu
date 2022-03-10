/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>

/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
SYSCALL scount(sem)
	int	sem;
{

	if (isbadsem(sem) || semaph[sem].sstate==SFREE)
		return(SYSERR);
	return(semaph[sem].semcnt);
}
