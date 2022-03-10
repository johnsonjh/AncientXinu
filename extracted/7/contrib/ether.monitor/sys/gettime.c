/* gettime.c - gettime, getutim */

#include "../h/conf.h"
#include "../h/kernel.h"
#include "../h/sleep.h"
#include "../h/date.h"
#include "../h/network.h"

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
SYSCALL	gettime(timvar)
long	*timvar;
{
	long	now;

	if (getutim(&now) == SYSERR)
		return(SYSERR);
	*timvar = ut2ltim(now);			/* adjust for timezone	*/
	return(OK);
}



/*------------------------------------------------------------------------
 * getutim  --  obtain time in seconds past Jan 1, 1970, ut (gmt)
 *------------------------------------------------------------------------
 */
SYSCALL	getutim(timvar)
long	*timvar;
{
	int	dev;
	int	len;
	long	now;
	long	utnow;
	char	buff[40];

	wait(clmutex);
	if (clktime < SECPERHR) {	/* assume small numbers invalid	*/
		sprintf(buff, "%d.%d.%d.%d:%d", NETSERVE, UTIME);
		if ((dev=open(NETWORK, buff, ANYLPORT)) == SYSERR ||
		    control(dev,DG_SETMODE,DG_TMODE|DG_DMODE) == SYSERR) {
			panic("gettime: access failed");
			signal(clmutex);
			return(SYSERR);
		}
		write(dev, buff, 1);	/* send junk packet to prompt */
		if ( (len=read(dev,&utnow,4)) == SYSERR) {
			signal(clmutex);
			return(SYSERR);
		}
		clktime = net2xt( net2hl(utnow) );
		close(dev);
	}
	*timvar = clktime;
	signal(clmutex);
	return(OK);
}
