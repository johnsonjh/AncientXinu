/* dsinter.c - dsinter */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>
#include <dir.h>

int	PanicOnError = TRUE;
/*------------------------------------------------------------------------
 *  dsinter  --  process disk interrupt (DTC interface; XEBEC controller)
 *------------------------------------------------------------------------
 */
INTPROC	dsinter(dsptr)
	struct	dsblk	*dsptr;
{
	struct	dtc	*dtptr;
	struct	dreq	*drptr;
	char	status;

#ifdef	DEBUG
	dotrace("dsinter", &dsptr, 1);
#endif
	dtptr = dsptr->dcsr;
	drptr = dsptr->dreqlst;
	if (drptr == DRNULL) {
		panic("Disk interrupt when disk not busy");
		return;
	}
	if ( (status = dtptr->dt_ccsr) & DTERROR ) {
		if( PanicOnError ) {
			kprintf("DISK ERROR\n");
			dsdump(dsptr, drptr);
		}
		drptr->drstat = SYSERR;
	} else
		drptr->drstat = OK;
	if ( (dsptr->dreqlst=drptr->drnext) != DRNULL)
		dskstrt(dsptr);
	switch (drptr->drop) {

		case DREAD:
		case DSYNC:
			ready(drptr->drpid, RESCHYES);
			return;

		case DWRITE:
			freebuf(drptr->drbuff);

		case DSEEK:
			freebuf(drptr);
	}
}

LOCAL	dsdump(dsptr, drptr)
struct	dsblk	*dsptr;
struct	dreq	*drptr;
{
	struct	dir	*dirptr;

	kprintf("\nDSBLK\n");
	kprintf("num open files=   %d\n", dsptr->dnfiles);
	kprintf("directory add = 0x%x\n", dsptr->ddir);
	kprintf("\nDREQLIST\n");
	kprintf("block address = 0x%x\n", drptr->drdba);
	kprintf("pid           =   %d\n", drptr->drpid);
	kprintf("buff address  = 0x%x\n", drptr->drbuff);
	kprintf("\nXEBEC\n");
	kprintf("operation     =   %d\n", dsptr->ddcb.xop);
	kprintf("block         = 0x%x\n", 
		(dsptr->ddcb.xmaddr <<8) | (dsptr->ddcb.xladdr));
	kprintf("\nDIR\n");
	dirptr = (struct dir *)dsptr->ddir;
	kprintf("num i-blocks  =   %d\n", dirptr->d_iblks);
	kprintf("free blk ptr  =   %d\n", dirptr->d_fblst);
	kprintf("free iblk ptr =   %d\n", dirptr->d_filst);
	kprintf("disk id       =   %d\n", dirptr->d_id);
	kprintf("num files     =   %d\n", dirptr->d_nfiles);
}
