/* ndump.c - ndump */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <name.h>

#define	PADTO	24

/*------------------------------------------------------------------------
 *  ndump  -  dump current mappings in object naming system
 *------------------------------------------------------------------------
 */
ndump()
{
	struct	nament	*nptr;
	int	i, len, dev;
	char	*p;

	for (i=0 ; i<Nam.nnames ; i++) {
		nptr = & Nam.nametab[i];
		kprintf("\"%-s\"", nptr->npre);
		for (len=strlen(nptr->npre) ; len < PADTO ; len++)
			kprintf(" ");
		dev = nptr->ndev;
		if (isbaddev(dev))
			p = "SYSERR";
		else
			p = devtab[dev].dvname;
		kprintf(" -> (%-8s) \"%s\"\n", p, nptr->nrepl);
	}
}
