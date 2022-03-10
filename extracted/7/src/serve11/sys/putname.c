/* putname.c - putname */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <network.h>
#include <serve11.h>


/*------------------------------------------------------------------------
 *  putname  -  look for ipaddr in name table, put it in if it isnt there
 *------------------------------------------------------------------------
 */
putname(ipaddr)
IPaddr ipaddr;
{
	static int machnum = 1;
	int i;
	Bool done;
	char *name;

	if ((name = (char *) getmem(NETNLEN)) == SYSERR) {
		panic("putname: out of memory");
		return(SYSERR);
		}
	for (i=0; i < mntabsize && i < MNTABSIZE; i++) 
		if (blkequ(mnametab[i].ipaddr, ipaddr, IPLEN))
			break;
	if (i == mntabsize && i < MNTABSIZE) {
		sprintf(name, "xinu%d", machnum++);

		/* look to see that this name does not already exist */
		done = FALSE;
		while (!done) {
			for (i=0 ; i<mntabsize ; i++) {
				if (!strcmp(name, mnametab[i].name)) {
					sprintf(name, "xinu%d", machnum++);
					break;
					}
				}
			if (i == mntabsize)
				done = TRUE;
			}

		strcpy(mnametab[mntabsize].name, name);
		blkcopy(mnametab[mntabsize].ipaddr, ipaddr, IPLEN);
		mntabsize++;
		}
	else if (i == mntabsize && i == MNTABSIZE) {
		kprintf("name table full\n");
		}
	freemem(name, NETNLEN);
}
