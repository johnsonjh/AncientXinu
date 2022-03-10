#include "/usr/include/stdio.h"
#include "/usr/include/sys/file.h"
#include "/usr/include/sys/types.h"
#include "/usr/include/sys/stat.h"

#include <freedev.h>
#include <getdev.h>

#define DEVNAME "/dev/%s"

freedev(class, request, user_rsrvd, flags)
char *class, *user_rsrvd;
int  request, flags;
{
	int retval;

	if ((flags & FD_RELES) && (flags & FD_DBENTRY))
		retval = freeD_reles(class, flags);
	else if (flags & FD_HOLD)
		retval = freeD_hold(class, request, user_rsrvd, flags);
	else if (flags & FD_FREE)
		retval = freeD_free(class, request, user_rsrvd, flags);
	else {
		fprintf(stderr, "freedev: unrecognizable request\n");
		retval = -1;
	}
	return(retval);
}

/*
 *=====================================================================
 * freeD_reles - release a physical device on reserve by somebody else
 *=====================================================================
 */
freeD_reles(phys, flags)
char *phys;
int  flags;
{
	char lknm[MAXNAMELEN], lkinfo[MAXRECLEN], *lkptr;
	char *user, *usr_lst, *get_ulock();
	int fdlk, reclen, fd_ulst, len;

	if (access(LOCKDIR, F_OK) == -1) {
		umask(0);
		mkdir(LOCKDIR, 0777);
	}
	sprintf(lknm, LOCKNAME, get_name(phys));
	if ((fdlk = open(lknm, O_RDONLY, 0)) < 0)
		return;		/* not on reserve */
	reclen = read(fdlk, lkinfo, MAXRECLEN);
	close(fdlk);
	lkinfo[reclen] = '\0';

	user = getuser(lkinfo);
	lkptr = skip_field(lkinfo);	/* move ptr to start of record,	*/
	lkptr = skip_white(lkptr);	/* so it is a db class entry	*/

	usr_lst = get_ulock(user, &len);
	if (len < 0)
		unlink(lknm);
	else {
		len = freeD_free(phys, 0, usr_lst, FD_DBENTRY);
		put_ulock(user, usr_lst, len);
	}
	return(0);
}

/*
 *==========================================================================
 * freeD_hold - release all devices held in user_rsrvd list except specific
 *    device requested.
 *==========================================================================
 */
freeD_hold(class, request, user_rsrvd, flags)
char *class;
int  request;
char *user_rsrvd;
int  flags;
{
	char lknm[MAXNAMELEN];
	char *phys, *devnm, *rsrvd_ptr;
	int found, firstfound, len, n;

	if (flags & FD_DBENTRY)
		phys = get_name(class);

	found = FALSE;
	firstfound = FALSE;
	len = 0;
	rsrvd_ptr = user_rsrvd;
	while(user_rsrvd != NULL  &&  *user_rsrvd != '\0') {
		if (flags & FD_DBENTRY) {
			if (strcmp(phys, get_phys(user_rsrvd)) == 0)
				found = TRUE;
		} else if (strcmp(class, get_class(user_rsrvd)) == 0
		       && ( ( n = atoi(get_num(user_rsrvd)) ) == request ||
			     request == FD_ANYDEV) ) {
				found = TRUE;
				request = n;
		}

		if (!found) {	/* free this entry */
			devnm = (flags & FD_DBENTRY) ? phys :
						       get_phys(user_rsrvd);
			sprintf(lknm, LOCKNAME, devnm);
			unlink(lknm);
		} 

		if (found && !firstfound) {/* first entry found--saveit */
			while (*user_rsrvd != NEWLINE) {
				*rsrvd_ptr++ = *user_rsrvd++;
				len++;
			}
			len++;	/* count the newline */
			*rsrvd_ptr++ = *user_rsrvd++;
			firstfound = TRUE;
		} else
			user_rsrvd = nextdb_entry(user_rsrvd);
		found = FALSE;
	}
	*rsrvd_ptr = '\0';
	return(len);
}

/*
 *========================================
 * freeD_free - free the specified device
 *========================================
 */
freeD_free(class, request, user_rsrvd, flags)
char *class;
int  request;
char *user_rsrvd;
int  flags;
{
	char *phys, *devnm, *list_ptr, *remove_entry;
	char *prev_ptr, *copy_ptr, lknm[MAXNAMELEN], *dbent;
	int found, len;

	if (flags & FD_DBENTRY)
		phys = get_name(class);

	remove_entry = NULL;
	list_ptr = user_rsrvd;
	len = 0;
	while(user_rsrvd != NULL  &&  *user_rsrvd != '\0') {
		if (flags & FD_DBENTRY) {
			if (strcmp(phys, get_phys(user_rsrvd)) == 0)
				remove_entry = user_rsrvd;
		} else if ( class == NULL  ||  strcmp(class,
						   get_class(user_rsrvd)) ) {
			if ( request == FD_ANYDEV  ||
			     request == atoi(get_num(user_rsrvd)) )
				remove_entry = user_rsrvd;
		}

		prev_ptr = user_rsrvd;
		user_rsrvd = nextdb_entry(user_rsrvd);

		if (remove_entry != NULL) {
			if (!(flags & FD_DBENTRY)) {
				devnm = get_phys(remove_entry);
				sprintf(lknm, LOCKNAME, devnm);
				unlink(lknm);
				if (flags & FD_VERBOSE) {
					dbent = lookup_db(devnm);
					printf("Released %s #%d (%s)\n",
						get_class(remove_entry),
						atoi(get_num(remove_entry)),
						shortname(dbent) );
				}
			}

			copy_ptr = remove_entry;
			while (*user_rsrvd != '\0')
				*remove_entry++ = *user_rsrvd++;
			*remove_entry = *user_rsrvd;
			user_rsrvd = copy_ptr;
			remove_entry = NULL;
		} else
			len += user_rsrvd - prev_ptr;
	}

	if (flags & FD_DBENTRY) {
		sprintf(lknm, LOCKNAME, phys);
		unlink(lknm);
	}
	return(len);
}
