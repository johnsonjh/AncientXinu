/* sleep.h */

#define	CVECTOR	0100		/* location of clock interrupt vector	*/

extern	int	clkruns;	/* 1 iff clock exists; 0 otherwise	*/
				/* Set at system startup.		*/
extern	int	clockq;		/* q index of sleeping process list	*/
extern	int	count6;		/* used to ignore 5 of 6 interrupts	*/
extern	int	count10;	/* used to ignore 9 of 10 ticks		*/
extern	long	clktime;	/* current time in secs since 1/1/70	*/
extern	int	clmutex;	/* mutual exclusion sem. for clock	*/
extern	int	*sltop;		/* address of first key on clockq	*/
extern	int	slnempty;	/* 1 iff clockq is nonempty		*/

extern	int	defclk;		/* >0 iff clock interrupts are deferred	*/
extern	int	clkdiff;	/* number of clock ticks deferred	*/
extern	int	clkint();	/* clock interrupt handler		*/
