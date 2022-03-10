/* fserver.c - fserver */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <serve11.h>

/*------------------------------------------------------------------------
 *  fserver  -  provide (limited) file server for the network
 *------------------------------------------------------------------------
 */
PROCESS fserver()
{
	struct	fpacket *fptr;
	int	devid, size, maxpacket;
	char	*msg;

	maxpacket = FPPLEN;

	if ( (devid=open(INTERNET, ANYFPORT, URFILE)) == SYSERR) {
		kprintf("\nRemote File Serve - cannot open.\n");
		return(SYSERR);
		}
	if ((msg = (char *) getmem(maxpacket)) == SYSERR) {
		panic("fserver: out of memory");
		return(SYSERR);
		}
	while ( TRUE ) {
		if ((size = read(devid, msg, maxpacket)) == SYSERR) {
			continue;
			} 
		fptr = (struct fpacket *) ((struct xgram *)msg)->xg_data;
		if (net2hs(fptr->fp_h.f_op) < 0) {
			continue;
			} 
		fptr->fp_h.f_op = hs2net(FS_ERROR);

		if (write(devid, msg, size) == SYSERR) {
               		panic("fserver -- cannot write");
               		}
		}
}
