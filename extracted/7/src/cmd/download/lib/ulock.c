#include "/usr/include/pwd.h"
#include "/usr/include/stdio.h"
#include "/usr/include/sys/types.h"
#include "/usr/include/sys/file.h"
#include "/usr/include/sys/stat.h"
#include "/usr/include/sys/time.h"
#include "/usr/include/sys/uio.h"


#include <getdev.h>
#include <freedev.h>
#include <ulock.h>

/*
 * get_ulock - return a ptr to the list of machines that the user currently
 *    holds.  Also, pass back the file desc. of the user lock file.
 */
char *get_ulock(loginid, len)
char *loginid;
int *len;
{
	char *ulocks;
	char usr_lock[MAXNAMELEN];
	int fdusr_lk;
	struct stat stat_buf;

	if (loginid == SELF)	/* else contains a user's id */
		if ( (loginid=getlogin()) == NULL || *loginid == '\0')
			loginid = (getpwuid(getuid()))->pw_name;
	sprintf(usr_lock, USRLOCK, loginid);
	if (access(LOCKDIR, F_OK) == -1) {
		umask(0);
		mkdir(LOCKDIR, 0777);
	}
	if ((fdusr_lk = open(usr_lock, O_RDWR | O_CREAT, LOCKMODE)) < 0) {
		strncat(usr_lock, ":get_ulock", MAXNAMELEN-strlen(usr_lock));
		perror(usr_lock);
		return(NULL);
	}
	fchmod(fdusr_lk, 0666);

	flock(fdusr_lk, LOCK_EX);

	if (fstat(fdusr_lk, &stat_buf) < 0) {	/* need len of ulock file */
		perror("get_locks -- lockfile stat");
		return(NULL);
	}
	*len = stat_buf.st_size;
	ulocks = (char *) malloc(*len + 1);
	if (read(fdusr_lk, ulocks, *len) < 0) {
		perror("get_locks -- user locks read");
		return(NULL);
	}
	close(fdusr_lk);
	ulocks[*len] = '\0';
	return(ulocks);
}

put_ulock(loginid, user_rsrvd, len)
char *loginid;
char *user_rsrvd;
int  len;
{
	char usr_lock[MAXNAMELEN];
	int fdusr_lk;

	if (loginid == SELF)	/* else contains a user's id */
		if ( (loginid=getlogin()) == NULL || *loginid == '\0')
			loginid = (getpwuid(getuid()))->pw_name;
	sprintf(usr_lock, USRLOCK, loginid);
	if (access(LOCKDIR, F_OK) == -1) {
		umask(0);
		mkdir(LOCKDIR, 0777);
	}
	fdusr_lk = open(usr_lock, O_RDWR | O_TRUNC | O_CREAT, LOCKMODE);
	fchmod(fdusr_lk, 0666);
	write(fdusr_lk, user_rsrvd, len);

	flock(fdusr_lk, LOCK_UN);
	close(fdusr_lk);

	if (*user_rsrvd == '\0')
		unlink(usr_lock);
}

/*
 * reorder_uclass - arrange the list of devices currenty on reserve
 *    by the user so that any devs in the listed class come first, with
 *    the specific device itself in the front of them. return len of list.
 */
/*
 * idea: create an iovec array that will avoid having to copy
 * the actual data around.  Just figure out where the critical
 * ptrs are.
 */
put_ureorder(loginid, rsrvd_lst, clmap_entry, user_rlen)
char *loginid;
char *rsrvd_lst;
char *clmap_entry;
int user_rlen;
{
	struct iovec iov[5];
	char *start_class, *next_class, *start_num, *end_num;
	char usr_lock[MAXNAMELEN];
	int fdusr_lk, entry_len;

	if (loginid == SELF)	/* else contains a user's id */
		if ( (loginid=getlogin()) == NULL || *loginid == '\0')
			loginid = (getpwuid(getuid()))->pw_name;
	sprintf(usr_lock, USRLOCK, loginid);

	fdusr_lk = open(usr_lock, O_RDWR | O_TRUNC | O_CREAT, LOCKMODE);
	fchmod(fdusr_lk, 0666);
	entry_len = index(clmap_entry, NEWLINE) - clmap_entry + 1;

	start_class = find_class(get_class(clmap_entry), rsrvd_lst);
	if (start_class == NULL  ||  *start_class == '\0') {
		write(fdusr_lk, clmap_entry, entry_len);
		write(fdusr_lk, rsrvd_lst, user_rlen);

		flock(fdusr_lk, LOCK_UN);
		close(fdusr_lk);

		return;
	}
	next_class = eo_class(start_class) + 1;
	start_num = find_num(atoi(get_num(clmap_entry)), start_class);
	if (start_num == NULL  ||  *start_num == '\0') {
		write(fdusr_lk, clmap_entry, entry_len);
		iov[0].iov_base = start_class;
		iov[0].iov_len = next_class - start_class;
		iov[1].iov_base = rsrvd_lst;
		iov[1].iov_len = start_class - rsrvd_lst;
		iov[2].iov_base = next_class;
		iov[2].iov_len = user_rlen - (next_class - rsrvd_lst);
		writev(fdusr_lk, iov, 3);

		flock(fdusr_lk, LOCK_UN);
		close(fdusr_lk);

		return;
	}
	end_num = eo_entry(start_num) + 1;
	iov[0].iov_base = start_num;
	iov[0].iov_len = end_num - start_num;
	iov[1].iov_base = start_class;
	iov[1].iov_len = start_num - start_class;
	iov[2].iov_base = end_num;
	iov[2].iov_len = next_class - end_num;
	iov[3].iov_base = rsrvd_lst;
	iov[3].iov_len = start_class - rsrvd_lst;
	iov[4].iov_base = next_class;
	iov[4].iov_len = user_rlen - (next_class - rsrvd_lst);
	writev(fdusr_lk, iov, 5);

	flock(fdusr_lk, LOCK_UN);
	close(fdusr_lk);

	return;
}
