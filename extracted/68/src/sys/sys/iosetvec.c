/* iosetvec.c - iosetvec */

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  iosetvec  -  fill in interrupt vectors and dispatch table entries
 *------------------------------------------------------------------------
 */
iosetvec(descrp, incode, outcode)
int	descrp;
int	incode;
int	outcode;
{
	struct	devsw	*devptr;
	struct	intmap	*map;
	struct	vector	*vptr;

#ifdef	DEBUG
	dotrace("iosetvec", &descrp, 3);
#endif
#ifdef	NDEVS
	if (isbaddev(descrp))
		return(SYSERR);
	devptr = &devtab[descrp];
	map = &intmap[devptr->dvnum];	/* fill in interrupt dispatch	*/
	map->iin =  devptr->dviint;	/*   map with addresses of high-*/
	map->icode = incode;		/*   level input and output	*/
	map->iout = devptr->dvoint;	/*   interrupt handlers and	*/
	map->ocode = outcode;		/*   minor device numbers	*/

#ifdef	lsi11
	vptr = (struct vector *) devptr->dvivec;
	vptr->vproc = (char *)INTVECI;	/* fill in input interrupt	*/
	vptr->vps = descrp | DISABLE ;	/*   vector PC and PS values	*/
	vptr = (struct vector *) devptr->dvovec;
	vptr->vproc = (char *)INTVECO;	/* fill in output interrupt	*/
	vptr->vps = descrp | DISABLE ;	/* vector PC and PS values	*/
#else
	vptr = &disptab[devptr->dvivec];
	vptr->vproc = (char *)INTVECI;	/* fill in input interrupt	*/
	vptr->vps = descrp;		/*   vector PC and arg. values	*/
	vptr = &disptab[devptr->dvovec];
	vptr->vproc = (char *)INTVECO;	/* fill in output interrupt	*/
	vptr->vps = descrp ;		/*   vector PC and arg. values	*/
#endif
	return(OK);
}
