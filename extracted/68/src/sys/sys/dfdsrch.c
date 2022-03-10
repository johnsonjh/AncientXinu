/* dfdsrch.c - dfdsrch */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  dfdsrch  --  search disk directory for position of given file name
 *------------------------------------------------------------------------
 */
dfdsrch(dsptr, filenam, mbits)
struct	dsblk	*dsptr;
char	*filenam;
int	mbits;
{
	struct	dir	*dirptr;
	struct	fdes	*fdptr;
	int	len;
	int	i;
	int	inum;

#ifdef	DEBUG1
	dotrace("dfdsrch", &dsptr, 3);
#endif
	if ( (len=strlen(filenam))<=0 || len>=FDNLEN)
		return(SYSERR);
	dirptr = dsdirec(dsptr->dnum);
	for (i=0 ; i<dirptr->d_nfiles ; i++)
		if (strcmp(filenam, dirptr->d_files[i].fdname) == 0)
			if ( (mbits&FLNEW) != 0)
				return(SYSERR);
			else
				return((int)&dirptr->d_files[i]);
	wait(dsptr->ddirsem);
	if ( (mbits&FLOLD) || dirptr->d_nfiles >= NFDES) {
		signal(dsptr->ddirsem);
		return(SYSERR);
	}
	inum = ibnew(dsptr->dnum, IBNWDIR);
	fdptr = &(dirptr->d_files[dirptr->d_nfiles++]);
	fdptr->fdlen = 0L;
	strcpy(fdptr->fdname, filenam);
	fdptr->fdiba = inum;
	write(dsptr->dnum, dskbcpy(dirptr), DIRBLK);
	signal(dsptr->ddirsem);
	return((int)fdptr);
}
