/* getlsi.c  -  procedure that automatically selects and LSI 11	*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "/usr/Xinu/include/getlsi.h"

#define	MAXSTR	30

char	*tmpnam = TMPNAME;

char	tmpuser[MAXSTR];
char	devname[MAXSTR];
char	convert[MAXSTR];
char	*loginid;
char	*lsidevs[] = {
#include "/usr/Xinu/include/lsidevs.h"
		"/dev/LSI"};
#define	NDEVS	sizeof(lsidevs)/sizeof(char *)


/************************************************************************/
/*									*/
/*  getlsi  -- get the LSI a user has already reserved, or one that is	*/
/*		free							*/
/*									*/
/************************************************************************/

getlsi(usethis, lnum, lsifd, locknm)
int	usethis;		/* specific LSI, "my" LSI, or "any" LSI	*/
int	*lnum;			/* place to store # of lsi allocated	*/
int	*lsifd;			/* place to store lsi file descriptor	*/
char	*locknm;		/* place to store lock file name	*/
{
	char	*getlogin();
	int	fd;
	char	c;

	if ( (loginid=getlogin()) == NULL)
		loginid = (getpwuid(getuid()))->pw_name;
	strcpy(tmpuser, tmpnam);
	strcat(tmpuser, loginid);
	*lnum = -1;
	*locknm = '\0';

	/* either get specified LSI, the one this user has reserved,	*/
	/*  or any LSI, depending on 'usethis'				*/

	if (usethis == MYLSI) {
		oldlsi(lnum,locknm);
	} else if (usethis == ANYLSI) {
		if  (oldlsi(lnum,locknm) < 0)
			newlsi(usethis,lnum,locknm);
	} else
		newlsi(usethis,lnum,locknm);

	/* if successful, *lnum contains (nonnegative) number of lsi	*/
	/*  that was allocated.  Else it is -1				*/

	if (*lnum >= 0) {
		if ( (*lsifd = open(lsidevs[*lnum],2)) < 0) {
		    fprintf(stderr,"Cannot open %s\n",lsidevs[*lnum]);
		    unlink(locknm);
		    unlink(tmpuser);
		    return(-1);
		}
		touch(locknm);
		return(*lnum);
	}
	if (usethis >= 0)
		fprintf(stderr, "LSI #%d unavailable.\n", usethis);
	else if (usethis == MYLSI)
		fprintf(stderr,"No LSI on reserve for you.\n");
	else
		fprintf(stderr, "No LSI 11 available.\n");
	return(-1);
}


/*
 *  oldlsi  --  return id of LSI already allocated to this user
 */
oldlsi(lnum, locknm)
int	*lnum;			/* place to store lsi number		*/
char	*locknm;		/* place to store lock file name	*/
{
	int	fd;
	int	lsinum;
	int	len;

	*lnum = -1;
	if ( (fd=open(tmpuser,2)) >= 0) {	/* user has a lock file	*/
		len = read(fd, &lsinum, sizeof(int));
		close(fd);
		if (len != sizeof(int) || lsinum<0  || lsinum>=NDEVS) {
			unlink(tmpuser);
			return(-1);
		}
		sprintf(convert, "%d", lsinum);
		strcpy(locknm, tmpnam);
		strcat(locknm, convert);
		if (access(locknm,4|2) < 0)
			return(-1);
		*lnum = lsinum;
		return(lsinum);
	}
	return(-1);
}

newlsi(usethis, lnum, locknm)
int	usethis;		/* force using this lsi			*/
int	*lnum;			/* place to store lsi number used	*/
char	*locknm;		/* place to store lock file name	*/
{
	int	i, fd;

	if (usethis >= 0) {
		if (trylsi(usethis, locknm) >= 0)
			return( *lnum = usethis );
		return( *lnum = -1);
	}
	for (i=0 ; i<NDEVS ; i++) {
		if (trylsi(i, locknm) >= 0)
			return( *lnum = i );
	}
	return( *lnum = -1 );
}


struct	stat	sbuf;

/*
 * trylsi -- see if a given lsi is available
 */
trylsi(lsinum, locknm)
int	lsinum;
char	*locknm;
{
	long	time();
	long	now;
	int	fd;

	if (access(lsidevs[lsinum], 0) < 0)
		return(-1);
	strcpy(locknm, tmpnam);
	sprintf(convert, "%d", lsinum);
	strcat(locknm, convert);
	if ((fd=creat(locknm,0600)) < 0) {
		stat(locknm, &sbuf);
		now = time(0L);
		if ( (int)(now-sbuf.st_mtime) > 600)
			unlink(locknm);
		else
			return(-1);
		if ((fd=creat(locknm,0600)) < 0) {
			fprintf(stderr,"Can't create lock\n");
			exit(-1);
		}
	}
	close(fd);
	strcpy(tmpuser, tmpnam);
	strcat(tmpuser, loginid);
	unlink(tmpuser);
	if ( (fd=creat(tmpuser,0666)) < 0) {
		perror("Can't create tmp userid file");
		exit(1);
	}
	write(fd, &lsinum, sizeof(int));
	close(fd);
	return(lsinum);
}

touch(locknm)
char	*locknm;			/* name of file to touch	*/
{
	close(creat(locknm, 0600));
}
