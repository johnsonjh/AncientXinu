#include "/usr/include/sys/file.h"
#include "/usr/include/sys/types.h"
#include "/usr/include/sys/stat.h"
#include <getdev.h>

main()
{
	int fd_db, fd, reclen, len, now, idle;
	char *db_buf, *db_ptr, lknm[MAXNAMELEN], lkinfo[MAXRECLEN + 1];
	char *user, *lkptr;
	struct stat stat_buf, sbuf;

	if ((fd_db = open(DEV_DB, O_RDONLY, 0)) < 0) {
		perror("get_db -- device database open");
		exit(1);
	}
	if (fstat(fd_db, &stat_buf) < 0) {	/* need len of db file */
		perror("get_db -- device database stat");
		exit(1);
	}
	len = stat_buf.st_size;
	db_buf = (char *) malloc(len + 1);
	if (read(fd_db, db_buf, len) < 0) {
		perror("get_db -- device database read");
		exit(1);
	}
	close(fd_db);
	db_buf[len] = '\0';

	db_ptr = skip_white(db_buf);
	while (*db_ptr != CLASS_SEPER  &&  *db_ptr != '\0') {
		if (*db_ptr != COMMENT) {
			sprintf(lknm, LOCKNAME, get_name(db_ptr));
			if ((fd = open(lknm, O_RDONLY, 0)) >= 0) {
				reclen = read(fd, lkinfo, MAXRECLEN);
				fstat(fd, &sbuf);
				close(fd);
				user = getuser(lkinfo);
				lkptr = skip_field(lkinfo);
				lkptr = skip_white(lkptr);
				lkinfo[reclen] = '\0';
				now = time(0L);
				idle = ( (int) (now-sbuf.st_mtime) ) / 60;
				printf("%10s idle time = %3d min class = %-6s reserved by %-10s\n",
					shortname(db_ptr), idle,
					get_class(lkptr), user);
			}
		}
		db_ptr = nextdb_entry(db_ptr);
	}
}
