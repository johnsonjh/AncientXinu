/* dudir.c - dudir */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <disk.h>
#include <lfile.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  dudir  --  print file system directory in readable form on CONSOLE
 *------------------------------------------------------------------------
 */
dudir(diskdev)
int	diskdev;
{
	DBADDR	firstdb, lastdb;
	struct	dir	*dirptr;
	struct	fdes	*fdptr;
	int	i, l;
	char	ps;

	if (isbaddev(diskdev))
		return(SYSERR);
	dirptr = dsdirec(diskdev);
	if (dirptr->d_nfiles < 0 || dirptr->d_nfiles > NFDES
			|| dirptr->d_iblks < 0) {
		kprintf("\n\nError: invalid directory format");
		kprintf(" (id=%d , #files=%d)\n",dirptr->d_id,
				dirptr->d_nfiles);
		return(SYSERR);
	}
	kprintf("\n***Directory     %d files      ", dirptr->d_nfiles);
	kprintf("(free lists: i=%d/%d, d=%d, id=%d)\n",
		dirptr->d_filst, dirptr->d_iblks,
		dirptr->d_fblst, dirptr->d_id);
	if (dirptr->d_nfiles == 0)
		return(OK);
	kprintf("\nFile        Name       Size   First iblock\n");
	for (i=0 ; i<dirptr->d_nfiles ; i++) {
		fdptr = &(dirptr->d_files[i]);
		kprintf("%3d       ",i+1);
		if ((l=strlen(fdptr->fdname))<=0 || l>=FDNLEN)
			kprintf("*ERROR*\n");
		else
			kprintf("%-10s%7D%11d\n",fdptr->fdname,
				fdptr->fdlen, fdptr->fdiba);
	}
	return(OK);
}
