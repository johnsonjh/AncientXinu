#include "/usr/include/stdio.h"
#include "/usr/include/sys/types.h"
#include "/usr/include/sys/file.h"
#include "/usr/include/sys/stat.h"
#include "/usr/include/ctype.h"
#include <getdev.h>

HashEnt *devhtab[HASHTAB_SIZE];

/*
 *=====================================================================
 * get_db - read in and init the LSI database.  Physical machines are
 *    hashed into the global devhtab hash table.  A pointer to the
 *    string of classes is returned.
 *=====================================================================
 */
char *get_db()
{
	int fd_db, len;
	char *db_buf, *db_ptr;
	struct stat stat_buf;

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
	while (*db_ptr != CLASS_SEPER) {
		if (*db_ptr != COMMENT)
			if ((db_ptr = insrt_dev(db_ptr)) == NULL)
				return(NULL);	/* End of Phys Entries */
			else
				db_ptr = skip_white(db_ptr);
		else
			db_ptr = nextdb_entry(db_ptr);
	}

	if (*(++db_ptr) == NEWLINE)	/* skip past section seperator */
		return(++db_ptr);
	else
		return(db_ptr);
}

/*
 *===============================================================
 * insrt_dev - insert the device string into a hash table entry
 *===============================================================
 */
char *insrt_dev(chdev_ptr)
char *chdev_ptr;
{
	int hval;
	HashEnt *hptr, *hnew;

	hnew = (HashEnt *) malloc(sizeof(HashEnt));
	hnew->machine = chdev_ptr;
	hnew->next_hash = NULL;

	hval = hash_name(get_name(chdev_ptr));
	if (devhtab[hval] == NULL)
		devhtab[hval] = hnew;
	else {
		hptr = devhtab[hval];
		while (hptr->next_hash != NULL)
			hptr = hptr->next_hash;
		hptr->next_hash = hnew;
	}

	if ((chdev_ptr = index(chdev_ptr, NEWLINE)) == NULL)
		return(NULL);
	else {
		*chdev_ptr = '\0';
		return(++chdev_ptr);
	}
}

/*
 *=========================================================================
 * lookup_db - lookup the entry for a db physical name. return dev string
 *=========================================================================
 */
char *lookup_db(pname)
char *pname;
{
	int hval;
	HashEnt *hptr;

	hval = hash_name(pname);
	hptr = devhtab[hval];
	while (hptr != NULL)
		if (strcmp(pname, get_name(hptr->machine)) == 0)
			return(hptr->machine);
		else
			hptr = hptr->next_hash;
	return(NULL);
}

/*
 * hash_name - return the hash value of the device
 */
hash_name(phys_name)
char *phys_name;
{
	int hashval = 0;

	while(*phys_name != '\0')
		hashval += *phys_name++;
	hashval %= HASHTAB_SIZE;
	return(hashval);
}

/*
 * skip_white - skip to the first non-white space char in a string
 */
char *skip_white(ch_ptr)
char *ch_ptr;
{
	if (ch_ptr == NULL)
		return(NULL);
	while (*ch_ptr != '\0'  &&  index(WHITE_SPACE, *ch_ptr) != 0)
		ch_ptr++;
	return(ch_ptr);
}

/*
 *=====================================================================
 * get_field - return ptr to a string holding a copy of the requested
 *    field from a line of text(null or newline terminated).
 *=====================================================================
 */
char *get_field(text, fld_count)
char *text;
int  fld_count;
{
	int  cur_count, i;
	char *field;

	for (cur_count = 1; cur_count < fld_count; cur_count++) {
		text = skip_field(text);
		text = skip_white(text);
		if (text == NULL  ||  *text == NEWLINE  ||  *text == '\0')
			return(NULL);
	}

	i = 0;
	field = (char *) malloc(MAXFIELDLEN);
	if (*text == QUOTE)
		while ((*++text != QUOTE) && *text != NULL)
			field[i++] = *text;
	else
		while (*text != NULL && index(FIELD_END, *text) == 0 &&
			(i + 1) < MAXFIELDLEN)
				field[i++] = *text++;
	field[i] = NULL;
	return(field);
}

/*
 *==========================================================================
 * find_class - return ptr to location of a class in the class/dev mapping
 *==========================================================================
 */
char *find_class(class, clmap)
char *class;
char *clmap;
{
	int len;

	len = strlen(class);
	while (clmap != NULL  &&  strncmp(class, clmap, len) != 0) {
		clmap = index(clmap, NEWLINE);
		if (clmap != NULL)
			clmap == skip_white(++clmap);
	}
	return(clmap);
}

/*
 *===========================================================================
 * find_num - return ptr to location of request in the current class in map
 *===========================================================================
 */
char *find_num(num, clmap)
int  num;
char *clmap;
{
	char *cur_class, *clfield;

	cur_class = get_class(clmap);
	while (clmap != NULL  &&  *clmap != '\0') {
		if (strcmp(get_class(clmap), cur_class) != 0)
			return(NULL);
		if (atoi(get_num(clmap)) == num)
			return(clmap);
		if ((clmap = index(clmap, NEWLINE)) != NULL)
			clmap++;
	}
	return(NULL);
}

/*
 * skip_field - return ptr to the next char after current field in db string
 */
char *skip_field(cfield)
char *cfield;
{
	while (cfield != NULL  &&  *cfield != '\0'
	       && index(WHITE_SPACE, *cfield) == 0)
		    cfield++;
	return(cfield);
}

/*
 * nextdb_entry - skip to next line of the data base
 */
char *nextdb_entry(db_ptr)
char *db_ptr;
{
	if ((db_ptr = index(db_ptr, NEWLINE)) == NULL)
		return(NULL);
	else
		return(skip_white(++db_ptr));
}

/*
 * eo_entry - return ptr to end of database entry
 */
char *eo_entry(db_ptr)
char *db_ptr;
{
	char *eoent;

	if ((eoent = index(db_ptr, NEWLINE)) == NULL)
		eoent = index(db_ptr, '\0');
	return(eoent);
}

/*
 * eo_class - return ptr to last char in the current class
 */
char *eo_class(db_ptr)
char *db_ptr;
{
	char *class, *clptr;

	class = get_class(db_ptr);
	clptr = db_ptr;
	do {
		clptr = nextdb_entry(clptr);
		if (clptr == NULL)
			return(index(db_ptr, '\0'));
		else if (*clptr == '\0')
			break;
	} while (strcmp(class, get_class(clptr)) == 0);
	return(clptr - 1);
}

/*============================================================================
 * shorthname -- return hostname part of IP domain name for a machine
 *============================================================================
*/
char	*shortname(machine)
char	*machine;
{
	char	*hname, *pdot;

	hname = get_hname(machine);
	if ( !isdigit(*hname) && (pdot = index(hname, DOMAIN_SEP)) != NULL)
		*pdot = '\0';
	return(hname);
}
