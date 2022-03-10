/* getdev.c  -  procedure that automatically selects a device		*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "getdev.h"

char	tmpuser[MAXNAMELEN];	/* name of file containing name of	*/
				/*  user's currently locked device	*/
char	tmplock[MAXNAMELEN];	/* lock file name template		*/
char	devname[MAXNAMELEN];	/* device name template			*/
char	dev[MAXNAMELEN];	/* device name				*/


/************************************************************************/
/*									*/
/*  getdev  -- get the LSI a user has already reserved, or one that is	*/
/*		free							*/
/*									*/
/************************************************************************/

getdev(useclass, class, request, devfd, locknm)
int	useclass;		/* FALSE => use previous class if avail.*/
char	*class;			/* which group of machines?		*/
int	request;		/* # of machine to allocate, ANYDEV, ...*/
int	*devfd;			/* place to store device file descriptor*/
char	*locknm;		/* place to store lock file name	*/
{
	char	*getlogin();
	char	*loginid;
	char	oldclass[MAXNAMELEN];	/* class on reserve		*/
	int	machnum;

	*locknm = '\0';
	if ( (loginid=getlogin()) == NULL || *loginid == '\0')
		loginid = (getpwuid(getuid()))->pw_name;

	if (request != TTYNAME)
		sprintf(tmpuser, TMPUSER, TMPDIR, loginid);
	else if ((*devfd=open(class, 2)) >= 0)
		return(0);
	else {
		fprintf(stderr, "Cannot open device %s\n", class);
		return(-1);
	}

	if ((machnum=getclass(oldclass)) > 0 &&
			(!useclass || request == MYDEV))
		strcpy(class, oldclass);

	/* Create file name templates.					*/

	sprintf(tmplock, DEVNAME, TMPDIR, class);
	sprintf(devname, DEVNAME, DEVDIR, class);

	/* either get specified device, the one this user has reserved,	*/
	/*  or any device, depending on 'request'			*/

	switch (request) {
	case MYDEV:
		machnum = olddev(machnum, locknm);
		break;
	case ANYDEV:
		if ((useclass && strcmp(class, oldclass) != 0) ||
			(machnum=olddev(machnum, locknm)) < 0)
			machnum = newdev(request, locknm);
		break;
	default:
		machnum = newdev(request, locknm);
		break;
	}

	/* if successful, machnum contains (positive) number of lsi	*/
	/*  that was allocated.  Else it is -1				*/

	if (machnum > 0) {
		if ( (*devfd = open(dev, 2)) < 0) {
		    fprintf(stderr,"Cannot open %s\n",dev);
		    unlink(locknm);
		    unlink(tmpuser);
		    return(-1);
		}
		touch(locknm);
		return(machnum);
	}
	if (request > 0)
		fprintf(stderr, "%s #%d unavailable.\n", class, request);
	else if (request == MYDEV)
		fprintf(stderr,"No %s on reserve for you.\n", class);
	else if (request == ANYDEV)
		fprintf(stderr, "No %s available.\n", class);
	else
		fprintf(stderr, "Unintelligible request: %d\n", request);
	return(-1);
}


/*------------------------------------------------------------------------
 *  getclass  --  return the class and number of the last used machine
 *------------------------------------------------------------------------
 */
getclass(class)
char	*class;
{
	char	*rindex();
	int	fd;
	char	oldname[MAXNAMELEN];
	int	len;
	int	oldnum;

	if ( (fd=open(tmpuser, 2)) < 0)	/* user has no reservation	*/
		return(-1);
	len = read(fd, oldname, sizeof(oldname));
	close(fd);
	oldname[len - 1] = '\0';	/* last character was a newline	*/

	if (len < 3 || access(sprintf(dev, "%s/%s", DEVDIR, oldname), 0) < 0
		|| sscanf(rindex(oldname, '.'), ".%d", &oldnum) != 1) {
		unlink(tmpuser);
		return(-1);
	}
	strcpy(class, oldname);
	*rindex(class, '.') = '\0';
	return(oldnum);
}


/*
 *  olddev  --  return id of device already allocated to this user
 */
olddev(machnum, locknm)
int	machnum;		/* number of requested machine		*/
char	*locknm;		/* place to store lock file name	*/
{
	if (machnum < 0)
		return(-1);
	else
		return(trydev(machnum, locknm));
}

newdev(machnum, locknm)
int	machnum;		/* place to store lsi number used	*/
char	*locknm;		/* place to store lock file name	*/
{
	int	i;

	if (machnum >= 0)
		return(trydev(machnum, locknm));
	for (i = 1 ; access(sprintf(dev, devname, i), 0) == 0 ; i++) {
		if (trydev(i, locknm) >= 0)
			return(i);
	}
	return(-1);
}


struct	stat	sbuf;

/*
 * trydev -- see if a given device is available
 */
trydev(machnum, locknm)
int	machnum;
char	*locknm;
{
	long	time();
	long	now;
	int	fd;
	int	len;
	char	lname[MAXNAMELEN];
	char	oldlock[MAXNAMELEN];

	if (access(sprintf(dev, devname, machnum), 0) < 0)
		return(-1);
	if ((fd=creat(sprintf(locknm, tmplock, machnum), 0600)) < 0) {
		stat(locknm, &sbuf);
		now = time(0L);
		if ( (int)(now-sbuf.st_mtime) > TIMEOUT * 60)
			unlink(locknm);
		else
			return(-1);
		if ((fd=creat(locknm,0600)) < 0) {
			fprintf(stderr,"Can't create lock\n");
			exit(-1);
		}
	}
	close(fd);			/* device is locked		*/
	if ((fd=open(tmpuser, 2)) >= 0) { /* remove my previous lock	*/
		len = read(fd, lname, sizeof(lname));
		close(fd);
		lname[len - 1] = '\0';
		if (strcmp(locknm, sprintf(oldlock,"%s/%s",TMPDIR,lname))==0)
			return(machnum);
		else if (access(oldlock, 4|2) == 0)	/* it's my lock	*/
			unlink(oldlock);
	}
	unlink(tmpuser);
	if ( (fd=creat(tmpuser,0666)) < 0) {
		perror("Can't create tmp userid file");
		exit(1);
	}
	write(fd, dev + strlen(DEVDIR)+1, strlen(dev) - strlen(DEVDIR)-1);
	write(fd, "\n", 1);
	close(fd);
	return(machnum);
}


touch(locknm)
char	*locknm;			/* name of file to touch	*/
{
	close(creat(locknm, 0600));
}
