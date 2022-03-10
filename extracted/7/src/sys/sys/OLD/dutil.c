/* dutil.c - dutil */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <disk.h>
#include <lfile.h>
#include <dir.h>
#include <tty.h>

#define	TERMEOF		'\004'		/* interpreted as EOF at console*/
#define	DISKID		33		/* an arbitrary disk id number	*/
#ifndef	DISK0
#define	DISK0		4
#endif

struct	dir	*dirptr;		/* pointer to the in-core dir	*/
int	diskdev;			/* disk device operated on	*/
int	dsize;				/* disk size in blocks		*/
int	niblks;				/* number of i-blocks this disk	*/
char	buf[200];			/* buffer for input line	*/
char	dbuff[DBUFSIZ];			/* one disk buffer		*/
char	msg[] =
"Choose from: dir, read, write, create, or type help\n";
#define	PROMPT	"\ncmd: "
char	arg1[150];
char	arg2[150];
Bool	verbose;

struct	cmdname	{			/* list of valid commands	*/
	char	*cname;
	int	cargs;
	char	*cexpl;
} ducmds[] = {	"append",	2,	"append file1 onto file2",
		"create",	1,	"create a new (empty) file",
		"dir",		0,	"list directory",
		"exec",		1,	"execute a program from disk",
		"freeze",	1,	"freeze this program & save",
		"help",		0,	"print this page of info",
		"iblock",	1,	"print an iblock",
		"init",		1,	"initialize disk device number",
		"mkfs",		0,	"make a file system",
		"name",		2,	"rename file1 to file2",
		"quit",		0,	"quit running this program",
		"read",		1,	"read a file and display it",
		"size",		0,	"report the size of the disk",
		"trace",	1,	"trace a file's iblock list",
		"verbose",	1,	"set verbose output y or n",
		"write",	1,	"write a file from terminal",
		"zap",		1,	"remove a file from the disk",
		"?",		0,	"synonym for 'help'"};
#define	NCMDS	sizeof(ducmds)/sizeof(struct cmdname)
int	ncmds;


/*------------------------------------------------------------------------
 *  dutil  --  general disk utility program.
 *------------------------------------------------------------------------
 */
dutil()
{
	int	ret;

	kprintf("\n\n*** Disk Utility Program ***\n\n");
	diskdev = DISK0;
	verbose = FALSE;
	ncmds = NCMDS;
	kprintf("Starting with disk DISK0 (device id = %d)",diskdev);
	kprintf("         type '?' for help\n");
	dirptr = dsdirec(diskdev);
	sizeup(diskdev);
	while (1) {
		switch( getcmd() ) {

		case 0:		ret = duapp(diskdev, arg1, arg2);
				break;

		case 1:		ret = ducreat(diskdev, arg1);
				break;

		case 2:		ret = dudir(diskdev);
				break;

		case 3:		ret = duexec(diskdev, arg1);
				break;

		case 4: 	ret = dufreeze(diskdev, arg1);
				break;

		case 5: 	ret = duhelp();
				break;

		case 6:		ret = duiblk(diskdev, arg1);
				break;

		case 7:		ret = duinit(arg1);
				if (ret != SYSERR)
					diskdev = ret;
				break;

		case 8:		ret = dupmkfs(diskdev);
				break;

		case 9:		ret = duname(diskdev, arg1, arg2);
				break;

		case 10:	ret = duquit();
				break;

		case 11:	ret = duread(diskdev, arg1);
				break;

		case 12:	ret = dusize(diskdev);
				break;

		case 13:	ret = dutrace(diskdev, arg1);
				break;

		case 14:	ret = duverbose(arg1);
				break;

		case 15:	ret = duwrite(diskdev, arg1);
				break;

		case 16:	ret = duzap(diskdev, arg1);
				break;

		case 17:	ret = duhelp();
				break;

		default:	ret = SYSERR;

		}
		if (verbose)
			kprintf("Command completed with code %d\n",ret);
	}
}


/*------------------------------------------------------------------------
 *  duapp  --  append one file to the end of another
 *------------------------------------------------------------------------
 */
duapp(diskdev, from, to)
int	diskdev;
char	*from, *to;
{
	int	f1, f2;
	int	ich;

	if (verbose)
		kprintf("Appending files...\n");
	if ( (f2=open(diskdev,to,"rwo")) == SYSERR) {
		kprintf("Can't open file %s\n",to);
		return(SYSERR);
	}
	if (verbose)
		kprintf("device id for file '%s' is %d\n",to,f2);
	if (strcmp(from,"-") == 0)
		f1 = CONSOLE;
	else
		if ( (f1=open(diskdev,from,"ro")) == SYSERR) {
			kprintf("Can't open input file %s\n",from);
			close(f2);
			return(SYSERR);
		}
	if (verbose)
		kprintf("device id for file '%s' is %d\n",from,f1);
	while ( (ich=getc(f2)) != EOF)
		if (ich == SYSERR)
			kprintf("Error positioning file %s\n",to);
	while ( (ich=inchar(f1)) != EOF) {
		if (ich == SYSERR) {
			kprintf("Error reading file %s\n", from);
			close(f1);
			close(f2);
			return(SYSERR);
		}
		putc(f2, ich);
	}
	close(f1);
	close(f2);
	return(OK);
}


/*------------------------------------------------------------------------
 *  ducreat  --  create an empty disk file given its name
 *------------------------------------------------------------------------
 */
ducreat(diskdev, name)
int	diskdev;
char	*name;
{
	int	f1;

	if (verbose)
		kprintf("Creating file %s\n", name);
	if ( (f1=open(diskdev,name,"nr")) == SYSERR) {
		kprintf("Can't create new file %s\n", name);
		return(SYSERR);
	}
	close(f1);
	return(OK);
}


/*------------------------------------------------------------------------
 *  duexec  --  unfreeze a program and execute from disk
 *------------------------------------------------------------------------
 */
duexec(diskdir, name)
int	diskdir;
char	*name;
{
	int	f1;

	if ( (f1=open(diskdev, name, "ro")) == SYSERR) {
		kprintf("Can't frind program %s\n", name);
		return(SYSERR);
	}
	kprintf("Command not implemented\n");
	close(f1);
	return(SYSERR);
}


/*------------------------------------------------------------------------
 *  dufreeze  --  freeze this program and save it on disk
 *------------------------------------------------------------------------
 */
dufreeze(diskdev, name)
int	diskdev;
char	*name;
{
	int	f1;

	if ( (f1 = open(diskdev,name,"wn")) == SYSERR) {
		kprintf("Can't create file %s\n", name);
		return(SYSERR);
	}
	kprintf("Command not implemented\n");
	close(f1);
	return(SYSERR);
}


/*------------------------------------------------------------------------
 *  duhelp  -- print help message
 *------------------------------------------------------------------------
 */
duhelp()
{
	int	i, len;

	kprintf("\nCommands available are:\n\n");
	kprintf("   Command      #args     Description\n");
	for (i=0 ; i<ncmds ; i++) {
		kprintf("   %s",ducmds[i].cname);
		for (len=strlen(ducmds[i].cname) ; len<16 ; len++)
			kprintf(" ");
		kprintf("%d    %s\n",ducmds[i].cargs, ducmds[i].cexpl);
	}
	kprintf("\n");
	return(OK);		
}


/*------------------------------------------------------------------------
 *  duiblk  --  print an iblock given its index
 *------------------------------------------------------------------------
 */
duiblk(diskdev, arg)
int	diskdev;
char	*arg;
{
	struct	iblk	iblock;
	int	k, j;
	char	ch;
	IBADDR	ibnum;

	for (ibnum=0 ; (ch = *arg)!='\000' ; arg++) {
		if (ch<'0' || ch>'9')
			return(SYSERR);
		ibnum = 10*ibnum + (ch - '0');
	}
	if (ibnum < 0)
		return(SYSERR);
	ibget(diskdev, ibnum, &iblock);
	kprintf("iblock %d: offset=%U  next=%d\n\n   ",
		ibnum, iblock.ib_byte, iblock.ib_next);
	for (j=IBLEN-1 ; j>=0&&iblock.ib_dba[j]==DBNULL ; j--)
		;
	if (j < 0)
		kprintf("empty\n");
	else {
		for (k=0 ; k<=j ; k++)
			kprintf("%u ", iblock.ib_dba[k]);
		kprintf("\n");
	}
	return(OK);
}


/*------------------------------------------------------------------------
 *  duinit  --  initialize disk drive number and size
 *------------------------------------------------------------------------
 */
duinit(drive)
char	*drive;
{
	int	i,n;
	char	ch;

	n = 0;
	for (i=0 ; i<20 ; i++) {
		if ( (ch = *drive++) == '\000' || ch=='\n') {
			sizeup(n);
			return(n);
		} else {
			if (ch<'0' || ch>'9') {
				kprintf("Digit expected; got '%c'\n",ch);
				return(SYSERR);
			}
			n = 10*n + (ch - '0');
		}
	}
	kprintf("Argument too long.\n");
	return(SYSERR);
}


/*------------------------------------------------------------------------
 *  dupmkfs  -- make a file system from scratch
 *------------------------------------------------------------------------
 */
dupmkfs(diskdev)
int	diskdev;
{
	int	ret;

	if (verbose)
		kprintf("Making a file system...\n");
	ret = dumkfs(diskdev, niblks, dsize, DISKID, verbose);
	return(ret);
}

/*------------------------------------------------------------------------
 *  duname  -- rename one file into another
 *------------------------------------------------------------------------
 */
duname(diskdev, fname, tname)
int	diskdev;
char	*fname;
char	*tname;
{
	struct	fdes	*fdptr, *fdptr2;
	struct	dir	*dirptr;
	int	i, pos;
	int	sem;

	if (verbose)
		kprintf("Renaming file '%s' to '%s'\n", fname, tname);
	dirptr = dsdirec(diskdev);
	sem = ( (struct dsblk *)devtab[diskdev].dvioblk)->ddirsem;
	wait(sem);
	for (pos= -1,i=0 ; i<dirptr->d_nfiles ; i++) {
		fdptr = &dirptr->d_files[i];
		if (strcmp(fname, fdptr->fdname) == 0) {
			pos = i;
		} else if (strcmp(tname, fdptr->fdname) == 0) {
			kprintf("File '%s' already exists\n",tname);
			signal(sem);
			return(SYSERR);
		}
	}
	if (pos >= 0) {
		strcpy(dirptr->d_files[pos].fdname, tname);
		write(diskdev, dskbcpy(dirptr), DIRBLK);
		signal(sem);
		return(OK);
	}
	kprintf("File '%s' not found in directory\n", fname);
	signal(sem);
	return(SYSERR);
}


/*------------------------------------------------------------------------
 *  duquit  --  stop executing
 *------------------------------------------------------------------------
 */
duquit()
{
	int	i;

	if (verbose)
		kprintf("Exiting utility program...\n");
	for (i=1 ; i<NPROC ; i++)
		if (i != currpid)
			kill(i);
	kill(currpid);
}


/*------------------------------------------------------------------------
 *  duread  -- read a file and display on the terminal
 *------------------------------------------------------------------------
 */
duread(diskdev, fname)
int	diskdev;
char	*fname;
{
	int	f, ich;

	if (verbose)
		kprintf("Read file %s\n", fname);
	if ( (f=open(diskdev, fname, "ro")) == SYSERR) {
		kprintf("Can't open file %s\n", fname);
		return(SYSERR);
	}
	while( (ich=getc(f)) != EOF )
		if (ich == SYSERR) {
			kprintf("***ERROR ON READ***\n");
			return(SYSERR);
		} else
			kprintf("%c", ich);
	close(f);
	return(OK);
}


/*------------------------------------------------------------------------
 *  dusize  --  size the disk and print out the number of blocks
 *------------------------------------------------------------------------
 */
dusize()
{
	kprintf("Disk has %u blocks on it\n", dsize);
}


/*------------------------------------------------------------------------
 *  dutrace  --  trace the iblock list for a given file
 *------------------------------------------------------------------------
 */
dutrace(diskdev, fname)
int	diskdev;
char	*fname;
{
	int	f;
	int	inum, i, j,k;
	struct	iblk	iblock;
	struct	flblk	*flptr;

	if (verbose)
		kprintf("Tracing iblocks for file %s\n", fname);
	if ( (f=open(diskdev, fname, "ro")) == SYSERR) {
		kprintf("Can't open file %s\n", fname);
		return(SYSERR);
	}
	flptr = (struct flblk *)devtab[f].dvioblk;
	inum = flptr->fl_iba;
	for (i=0 ; inum!=IBNULL && i<10 ; i++) {
		kprintf("iblock %d: ",inum);
		ibget(diskdev, inum, &iblock);
		for (j=IBLEN-1 ; j>=0&&iblock.ib_dba[j]==DBNULL ; j--)
			;
		if (j < 0)
			kprintf("empty\n");
		else {
			for (k=0 ; k<=j ; k++)
				kprintf("%u ", iblock.ib_dba[k]);
			kprintf("\n");
		}
		inum = iblock.ib_next;
	}
	close(f);
	return(OK);
}

/*------------------------------------------------------------------------
 *  duverbose  --  turn on/off verbose output switch
 *------------------------------------------------------------------------
 */
duverbose(arg)
char	*arg;
{
	int	len;

	if (verbose)
		kprintf("Setting verbose option.\n");
	len = strlen(arg);
	if (len<=3 && strncmp(arg,"yes",len) == 0) {
		verbose = TRUE;
		return(OK);
	} else if (len<=2 && strncmp(arg,"no",len) == 0) {
		verbose = FALSE;
		return(OK);
	}
	kprintf("Argument must be 'yes' or 'no', not '%s'\n", arg);
	return(SYSERR);
}


/*------------------------------------------------------------------------
 *  duwrite  --  overwrite a file taking input from terminal
 *------------------------------------------------------------------------
 */
duwrite(diskdev, fname)
{
	int	f, ich;

	if (verbose)
		kprintf("Writing file %s\n", fname);
	if ( (f=open(diskdev,fname,"wo")) == SYSERR) {
		kprintf("Error opening file %s -- will attempt to create\n",
			fname);
		if ( (f=open(diskdev,fname,"wn")) == SYSERR) {
			kprintf("Can't create file %s\n", fname);
			return(SYSERR);
		}
	}
	kprintf("Enter text; terminate with CNTL-d\n\n");
	while( (ich=inchar(CONSOLE)) != EOF )
		putc(f, ich);
	close(f);
	return(OK);
}


/*------------------------------------------------------------------------
 *  dupzap  --  zap a file from the directory
 *------------------------------------------------------------------------
 */
duzap(diskdev, fname)
int	diskdev;
char	*fname;
{
	struct	fdes	*fdptr, *fdptr2;
	struct	dir	*dirptr;
	int	i, j, sem;
	IBADDR	ilist;

	if (verbose)
		kprintf("Removing file %s from directory\n", fname);
	dirptr = dsdirec(diskdev);
	sem = ((struct dsblk *)devtab[diskdev].dvioblk)->ddirsem;
	wait(sem);
	for (i=0 ; i<dirptr->d_nfiles ; i++) {
		fdptr = &dirptr->d_files[i];
		if (strcmp(fname, fdptr->fdname) == 0) {
			ilist = fdptr->fdiba;
			dirptr->d_nfiles--;
			fdptr2 = &dirptr->d_files[i+1];
			for (j=i ; j<dirptr->d_nfiles ; j++)
				*fdptr++ = *fdptr2++;
			write(diskdev, dskbcpy(dirptr), DIRBLK);
			signal(sem);
			iblfree(diskdev, ilist);
			return(OK);
		}
	}
	kprintf("File %s not found in directory\n", fname);
	signal(sem);
	return(SYSERR);
}


/*------------------------------------------------------------------------
 *  sizeup  --  size up the disk and place number of blocks in dsize
 *------------------------------------------------------------------------
 */
LOCAL	sizeup(diskdev)
int	diskdev;
{
	DBADDR	low, high, mid;

	low = 0;
	high = 31000;
	while (low < high-1) {
		mid = (DBADDR)(low + high)/(DBADDR)2;
		if (read(diskdev,dbuff,mid) == SYSERR)
			high = mid;
		else
			low = mid;
	}
	dsize = low+1;
	niblks = dsize/20;
}


/*------------------------------------------------------------------------
 *  inchar  --  get next input character
 *------------------------------------------------------------------------
 */
LOCAL	inchar(dev)
{
	int	ich;

	if (dev!=CONSOLE)
		return(getc(dev));
	if ( (ich=getc(CONSOLE)) == TERMEOF) {
		while( getc(CONSOLE) != NEWLINE)
			;
		return(EOF);
	} else
		return(ich);
}


/*------------------------------------------------------------------------
 *  getcmd  -- get next command and appropriate number of args.
 *------------------------------------------------------------------------
 */
getcmd()
{
	int	i;

	while (1) {
		kprintf("%s", PROMPT);
		i = duparse();
		sleep10(1);
		if (i != SYSERR)
			return(i);
		else 			
			kprintf("%s\n", msg);
	}
}


/*------------------------------------------------------------------------
 *  duparse  -- parse one command line and return command index
 *------------------------------------------------------------------------
 */
LOCAL	duparse()
{
	int	len, i, nargs, cindex;
	char	ch;

	while( (ch=getc(CONSOLE)) == ' ')
		;
	for (len=0 ; ch!='\n' && ch!=' ' ; ch=getc(CONSOLE))
		buf[len++] = ch;
	if (len == 0)
		return(SYSERR);
	buf[len] = '\000';
	if ( (cindex=lookup(len)) == SYSERR) {
		read(CONSOLE, buf, 0);
		return(SYSERR);
	}
	if ( (nargs=ducmds[cindex].cargs) == 0) {
		while (ch != '\n')
			ch = getc(CONSOLE);
		return(cindex);
	}
	while (ch == ' ')
		 ch=getc(CONSOLE);
	if (ch == '\n') {
		kprintf("\nToo few arguments for '%s'\n",
			ducmds[cindex].cname);
		return(SYSERR);
	}
	for (len=0 ; ch!='\n' && ch!=' ' ; ch=getc(CONSOLE))
		arg1[len++] = ch;
	arg1[len] = '\000';
	if ( (nargs=ducmds[cindex].cargs) <= 1) {
		while (ch != '\n')
			ch = getc(CONSOLE);
		return(cindex);
	}
	while (ch == ' ')
		 ch=getc(CONSOLE);
	if (ch == '\n') {
		kprintf("\nToo few arguments for '%s'\n",
			ducmds[cindex].cname);
		return(SYSERR);
	}
	for (len=0 ; ch!='\n' && ch!=' ' ; ch=getc(CONSOLE))
		arg2[len++] = ch;
	arg2[len] = '\000';
	while (ch != '\n')
		ch = getc(CONSOLE);
	return(cindex);
}


/*------------------------------------------------------------------------
 *  lookup  -- lookup a command name in the table, returning its index
 *------------------------------------------------------------------------
 */
LOCAL	lookup(len)
int	len;
{
	int	i;

	for (i=0 ; i<ncmds ; i++)
		if (len <= strlen(ducmds[i].cname) &&
		    strncmp(ducmds[i].cname, buf, len) == 0)
			return(i);
	return(SYSERR);
}
