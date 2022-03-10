/* sem.h - isbadsem */

#ifndef	NSEM
#define	NSEM		35	/* number of semaphores, if not defined	*/
#endif

#define	SFREE	'\01'		/* this semaphore is free		*/
#define	SUSED	'\02'		/* this semaphore is used		*/

struct	sentry	{		/* semaphore table entry		*/
	char	sstate;		/* the state SFREE or SUSED		*/
	short	semcnt;		/* count for this semaphore		*/
	short	sqhead;		/* q index of head of list		*/
	short	sqtail;		/* q index of tail of list		*/
};
extern	struct	sentry	semaph[];
extern	int	nextsem;

#define	isbadsem(s)	(s<0 || s>=NSEM)
