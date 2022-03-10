/* dfdsrch.c - dfdsrch */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <lfile.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  dfdsrch  --  search disk directory for position of given file name
 *------------------------------------------------------------------------
 */
struct	fdes	*dfdsrch(dsptr, filenam, mbits)
struct	dsblk	*dsptr;
char	*filenam;
int	mbits;
{
	struct	dir	*dirptr;
	struct	fdes	*fdptr;
	int	len;
	int	i;
	int	inum;

	if ( (len=strlen(filenam))<=0 || len>=FDNLEN)
		return((struct fdes *)SYSERR);
	dirptr = dsdirec(dsptr->dnum);
	for (i=0 ; i<dirptr->d_nfiles ; i++)
		if (strcmp(filenam, dirptr->d_files[i].fdname) == 0)
			if ( (mbits&FLNEW) != 0)
				return((struct fdes *)SYSERR);
			else
				return(&dirptr->d_files[i]);
	wait(dsptr->ddirsem);
	if ( (mbits&FLOLD) || dirptr->d_nfiles >= NFDES) {
		signal(dsptr->ddirsem);
		return((struct fdes *)SYSERR);
	}
	inum = ibnew(dsptr->dnum, IBNWDIR);
	fdptr = &(dirptr->d_files[dirptr->d_nfiles++]);
	fdptr->fdlen = 0L;
	strcpy(fdptr->fdname, filenam);
	fdptr->fdiba = inum;
	write(dsptr->dnum, dskbcpy(dirptr), DIRBLK);
	signal(dsptr->ddirsem);
	return(fdptr);
}
