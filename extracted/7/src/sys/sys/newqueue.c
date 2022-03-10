/* newqueue.c  -  newqueue */

#include <conf.h>
#include <kernel.h>
#include <q.h>

/*------------------------------------------------------------------------
 * newqueue  --  initialize a new list in the q structure
 *------------------------------------------------------------------------
 */
int	newqueue()
{
	struct	qent	*hptr;		/* address of new list head	*/
	struct	qent	*tptr;		/* address of new list tail	*/
	int	hindex, tindex;		/* head and tail indexes	*/

	hptr = &q[ hindex=nextqueue++ ];/* nextqueue is global variable	*/
	tptr = &q[ tindex=nextqueue++ ];/*  giving next used q pos.	*/
	hptr->qnext = tindex;
	hptr->qprev = EMPTY;
	hptr->qkey  = MININT;
	tptr->qnext = EMPTY;
	tptr->qprev = hindex;
	tptr->qkey  = MAXINT;
	return(hindex);
}
