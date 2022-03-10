/* kprintf.c - kprintf, kputc, savestate, rststate */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <slu.h>
#include <tty.h>

/*------------------------------------------------------------------------
 *  kprintf  --  kernel printf: formatted, unbuffered output to CONSOLE
 *------------------------------------------------------------------------
 */
kprintf(fmt, args)		/* Derived by Bob Brown, Purdue U.	*/
        char *fmt;
{
        int     kputc();

        savestate();
        _doprnt(fmt, &args, kputc, CONSOLE);
        rststate();
        return(OK);
}

/*------------------------------------------------------------------------
 *  kputc  --  write a character on the console using polled I/O
 *------------------------------------------------------------------------
 */
LOCAL	kputc(device ,c)
	int	device;
	register char c;	/* character to print from _doprnt	*/
{
	struct	csr	*csrptr;
/*T*/	int	slowdown;	/* added to delay output because VAX	*/
/*T*/				/* can't take it at 9600 baud		*/
	if ( c == 0 )
		return;
	if ( c == NEWLINE )
		kputc( device, RETURN );
	csrptr = (struct csr *)devtab[device].dvcsr;	/* dev.	address	*/
	while ( (csrptr->ctstat & SLUREADY) == 0 ) ;	/* poll for idle*/
	csrptr->ctbuf = c;
/*T*/	for(slowdown=0;slowdown<1000;slowdown++) ;
	while ( (csrptr->ctstat & SLUREADY) == 0 ) ;	/* poll for idle*/
}

LOCAL	int	savecrstat, savectstat;
LOCAL	char	saveps;
/*------------------------------------------------------------------------
 *  savestate  --  save the console control and status register
 *------------------------------------------------------------------------
 */
LOCAL	savestate()
{
	char ps;

	disable(ps);
	saveps = ps;
	savecrstat = (devtab[CONSOLE].dvcsr)->crstat & SLUENABLE;
	(devtab[CONSOLE].dvcsr)->crstat = SLUDISABLE;
	savectstat = (devtab[CONSOLE].dvcsr)->ctstat & SLUENABLE;
	(devtab[CONSOLE].dvcsr)->ctstat = SLUDISABLE;
}

/*------------------------------------------------------------------------
 *  rststate  --  restore the console output control and status register
 *------------------------------------------------------------------------
 */
LOCAL	rststate()
{
	char ps;

	(devtab[CONSOLE].dvcsr)->crstat = savecrstat;
	(devtab[CONSOLE].dvcsr)->ctstat = savectstat;
	ps = saveps;
	restore(ps);
}
