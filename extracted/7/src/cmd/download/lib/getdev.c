#include "/usr/include/pwd.h"
#include "/usr/include/stdio.h"
#include "/usr/include/sys/types.h"
#include "/usr/include/sys/file.h"
#include "/usr/include/sys/stat.h"
#include "/usr/include/sys/time.h"

#include <getdev.h>
#include <freedev.h>
#include <ulock.h>

#define DEVNAME "/dev/%s"
#define	DEFCLASS "LSI"

extern char **environ;
static char *user_rsrvd;

/*
 *===================================================================
 * getdev - get the lsi that the user requests.  Return a ptr to a
 *    DevDesc struct containing info about the allocated device.
 *===================================================================
 */
DevDesc *getdev(class, request, flags)
char *class;			/* which group of machines?		*/
int  request;			/* # of machine to allocate, ANYDEV, ...*/
int  flags;			/* options to control allocation	*/
{
	char *class_lst, *name;
	char devname[MAXNAMELEN];
	int  user_rlen;
	DevDesc *dev;

	class_lst = get_db();	/* ptr to start of class info */
	user_rsrvd = get_ulock(SELF, &user_rlen);
	if (class == NULL)
		if (user_rsrvd != NULL  &&  *user_rsrvd != '\0')
			class = get_class(user_rsrvd); /* one on reserve */
		else
			class = getenv(DEV_ENV);
	if (class == NULL)
		class = DEFCLASS;
	if (!(flags & GD_HLDDEVS))	/* free all but requested dev	*/
		user_rlen = freedev(class, request, user_rsrvd, FD_HOLD);
	
	if ((dev = onreserve(class, request, user_rsrvd)) == NULL)
		if (flags & GD_MYDEV)
			return(NULL);
		else
			dev = reserve(class, request, class_lst, flags);

	if (dev != NULL) {
		name = get_name(dev->machine);
		sprintf(devname, DEVNAME, name);
		if ((dev->fd = open(devname, O_RDWR, 0)) < 0) {
			fprintf(stderr, "Cannot open %s\n", devname);
			user_rlen = freedev(dev->machine, 0, user_rsrvd,
				FD_FREE | FD_DBENTRY);
			put_ulock(SELF, user_rsrvd, user_rlen);
			return(NULL);
		} else {
			put_ureorder(SELF, user_rsrvd, dev->class, user_rlen);
			return(dev);
		}
	}

	put_ulock(SELF, user_rsrvd, user_rlen);
	if (request > 0)
		fprintf(stderr, "%s #%d unavailable.\n", class, request);
	else if (request == GD_ANYDEV)
		fprintf(stderr, "No class %s machines available.\n", class);
	else
		fprintf(stderr, "Unintelligible request: %d\n", request);
	return(NULL);
}


/*
 *========================================================================
 * onreserve - rtrn device descrip if user already holds lock on request
 *========================================================================
 */
DevDesc *onreserve(class, request, rsrvd_ptr)
char *class;
int  request;
char *rsrvd_ptr;
{
	char *mach;
	DevDesc *dev_desc;

	rsrvd_ptr = find_class(class, rsrvd_ptr);		
	if (rsrvd_ptr == NULL  ||  rsrvd_ptr == '\0')
		return(NULL);

	if (request >= 0)
		if ((rsrvd_ptr = find_num(request, rsrvd_ptr)) == NULL)
			return(NULL);

	if ((mach = lookup_db(get_phys(rsrvd_ptr))) == NULL)
		return(NULL);
	
	dev_desc = touch_lock(mach);
	if (dev_desc != NULL)
		dev_desc->class = rsrvd_ptr; /*what if rsrvd list modified?*/
	return(dev_desc);
}
	
/*
 *===========================================================
 * reserve - lock an lsi from the specified class & request
 *===========================================================
 */
DevDesc *reserve(class, request, clist, flags)
char *class;
int  request;
char *clist;
int  flags;
{
	char *mach;
	DevDesc *dev_desc;

	clist = find_class(class, clist);
	if (clist == NULL)
		return(NULL);

	switch (request) {
	  case GD_ANYDEV:
		dev_desc = getany_dev(clist, flags);
		break;

	  default:
		dev_desc = getthis_dev(clist, request, flags);
		break;
	}
	if (dev_desc == NULL)
		return(NULL);

	/* dev_desc temporarily holds the fd of the lock file */
	init_lock(dev_desc->fd, dev_desc->class);
	dev_desc->fd = INVALID;
	return (dev_desc);	
}

/*
 * getany_dev - 
 */
DevDesc *getany_dev(clist, flags)
char *clist;
int flags;
{
	char *clptr, *class, *mach;
	DevDesc *dev_desc;

	class = get_class(clist);
	clptr = clist;
	while (clptr != NULL  &&  strcmp(class, get_class(clptr)) == 0) {
		if ((mach = lookup_db(get_phys(clptr))) != NULL) {
			dev_desc = create_lock(mach, flags);
			if (dev_desc != NULL) {
				dev_desc->class = clptr;
				return(dev_desc);
			}
		}
		clptr = nextdb_entry(clptr);
	}

	clptr = clist;		/* Check for expired time limits now */
	while (clptr != NULL  &&  strcmp(class, get_class(clptr)) == 0) {
		if ((mach = lookup_db(get_phys(clptr))) != NULL)
			if (atoi(get_limit(clptr)) <= idletime(mach)) {
				dev_desc = create_lock(mach, flags|GD_FORCE);
				if (dev_desc != NULL) {
					dev_desc->class = clptr;
					return(dev_desc);
				}
			}
		clptr = nextdb_entry(clptr);
	}
	return(NULL);
}

DevDesc *getthis_dev(clist, request, flags)
char *clist;		/* already points to correct class */
int  request;
int  flags;
{
	char *mach;
	DevDesc *dev_desc;

	clist = find_num(request, clist);
	if (clist == NULL)
		return(NULL);
	if ((mach = lookup_db(get_phys(clist))) == NULL)
		return(NULL);
	dev_desc = create_lock(mach, flags);
	if (dev_desc == NULL)
		if (atoi(get_limit(clist)) <= idletime(mach))
			dev_desc = create_lock(mach, flags|GD_FORCE);
	if (dev_desc != NULL)
		dev_desc->class = clist;
	return(dev_desc);
}

/*
 * idletime - return how long the machine has been idle
 */
idletime(mach)
char *mach;
{
	char lknm[MAXNAMELEN];
	struct stat stbuf;
	struct timeval tp;
	struct timezone tzp;
	int idle;

	sprintf(lknm, LOCKNAME, get_name(mach));
	if (stat(lknm, &stbuf) < 0)
		return(MAXTIME);
	gettimeofday(&tp, &tzp);
	idle = (tp.tv_sec - stbuf.st_mtime) / SECPERMIN;
	return(idle);
}
	
/*
 * create_lock - return device descriptor with machine and lockname
 *    filled in if possible to open the lock.
 */
DevDesc *create_lock(mach, flags)
char *mach;
int  flags;
{
	char *lknm;
	int fd;
	DevDesc *dev_desc;

	if (mach == NULL)
		return(NULL);

	if (access(LOCKDIR, F_OK) == -1) {
		umask(0);
		mkdir(LOCKDIR, 0777);
	}
	lknm = (char *) malloc(MAXNAMELEN);
	sprintf(lknm, LOCKNAME, get_name(mach));

	fd = open(lknm, O_CREAT | O_RDWR | O_EXCL, LOCKMODE);
	if (fd < 0  &&  flags & GD_FORCE) {
		freedev(mach, 0, NULL, FD_DBENTRY | FD_RELES);
		fd = open(lknm, O_CREAT | O_RDWR | O_EXCL, LOCKMODE);
	}

	if (fd >= 0) {
		fchmod(fd, 0666);
		dev_desc = (DevDesc *) malloc(sizeof(DevDesc));
		dev_desc->fd = fd;	/* temp hold lock fd in DevDesc blk */
		dev_desc->locknm = lknm;
		dev_desc->machine = mach;
		return(dev_desc);
	} else
		return(NULL);
}

/*
 * touch_lock - user already has the lock file assigned.  Just need
 *    to update info.  Return device desciptor with machine and
 *    lockname fields filled in.  
 */
DevDesc *touch_lock(mach, clmap)
char *mach;
char *clmap;
{
	char *locknm;
	int  fd_lk, slen;
	DevDesc *dev_desc;

	locknm = (char *) malloc(MAXNAMELEN);
	sprintf(locknm, LOCKNAME, get_name(mach));
	if ((fd_lk = open(locknm, O_RDWR, 0)) < 0)
		return(NULL);
	close(fd_lk);

	dev_desc = (DevDesc *) malloc(sizeof(DevDesc));
	dev_desc->locknm = locknm;
	dev_desc->machine = mach;
	return(dev_desc);
}

/*
 * init_lock - called after lock file is open, but no initialization of
 *    the locks have been done.  Enter user/class info in machine lock,
 *    and class/machine info in user lock file.
 */
init_lock(lk_fd, clmap)
int  lk_fd;
char *clmap;
{
	char *loginid;
	int  maplen, slen;

	if ( (loginid=getlogin()) == NULL || *loginid == '\0')
		loginid = (getpwuid(getuid()))->pw_name;

	if ((maplen = index(clmap, NEWLINE) - clmap) <= 0) {
		fprintf(stderr, "init_lock() -- database format incorrect\n");
		exit(1);
	}
	maplen++;	/* count the NEWLINE */

	/* init the actual lock for the device */
	slen = strlen(loginid);
	loginid[slen] = FIELDSEP;
	write(lk_fd, loginid, slen + 1);
	write(lk_fd, clmap, maplen);
	close(lk_fd);
}


touch(lknm)
char *lknm;
{
	int fd;
	char a;

	if (access(LOCKDIR, F_OK) == -1) {
		umask(0);
		mkdir(LOCKDIR, 0777);
	}
	fd = open(lknm, O_RDWR, 0);
	read(fd, &a, 1);
	lseek(fd, 0,0);
	write(fd, &a, 1);
	close(fd);
}
