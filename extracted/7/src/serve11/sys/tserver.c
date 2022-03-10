/* tserver.c - tserver */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <network.h>

#define	MAXTMSG	80
#define LINELEN 80

/*------------------------------------------------------------------------
 *  tserver  -  provide time server for the network
 *------------------------------------------------------------------------
 */
PROCESS tserver()
{
	int	devid;
	char	*msg;
        char	*line;
	long	utime;
	long	nettime;

	if ( (devid=open(INTERNET, ANYFPORT, UTIME)) == SYSERR ) {
		kprintf("\nTime Serve - cannot open.\n");
		return(SYSERR);
	}
	if ((msg  = (char *) getmem(MAXTMSG)) == SYSERR ||
	    (line = (char *) getmem(LINELEN))   == SYSERR) {
		panic("tserver: out of memory");
		return(SYSERR);
		}
	while ( TRUE ) {
		if (read(devid, msg, MAXTMSG) == SYSERR)
			continue;
		if (getutim(&utime) == SYSERR) {
			panic("tserver -- bad time");
			freemem(msg, MAXTMSG);
			freemem(line, LINELEN);
			return(SYSERR);
			}
                ascdate(ut2ltim(utime), line);
		nettime = hl2net(xt2net(utime));
                blkcopy(((struct xgram *)msg)->xg_data, 
                        (char *) (&nettime), sizeof(long));
		if (write(devid, msg, sizeof(long)) == SYSERR) {
                   panic("tserver -- cannot write");
                   }
	}
}
