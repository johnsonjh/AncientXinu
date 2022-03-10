/* ioinit.c - ioinit, iosetvec */

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  ioinit --  standard interrupt vector and dispatch initialization
 *------------------------------------------------------------------------
 */
ioinit(descrp)
int	descrp;
{
	int	minor;

	if (isbaddev(descrp) )
		return(SYSERR);
	minor = devtab[descrp].dvminor;
	iosetvec(descrp, minor, minor);
	return(OK);
}

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

	if (isbaddev(descrp))
		return(SYSERR);
	devptr = &devtab[descrp];
	map = &intmap[devptr->dvnum];	/* fill in interrupt dispatch	*/
	map->iin =  devptr->dviint;	/*   map with addresses of high-*/
	map->icode = incode;		/*   level input and output	*/
	map->iout = devptr->dvoint;	/*   interrupt handlers and	*/
	map->ocode = outcode;		/*   minor device numbers	*/
	vptr = (struct vector *)devptr->dvivec;
	vptr->vproc = (char *)INTVECI;	/* fill in input interrupt	*/
	vptr->vps = descrp | DISABLE;	/*   vector PC and PS values	*/
	vptr = (struct vector *)devptr->dvovec;
	vptr->vproc = (char *)INTVECO;	/* fill in output interrupt	*/
	vptr->vps = descrp | DISABLE;	/*   vector PC and PS values	*/
	return(OK);
}
