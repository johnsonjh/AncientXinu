/* kernelio.c - kgetc kputc kprintf */

#include <kernel.h>
#include <uart.h>
#include <necuart.h>
#include <tty.h>

#define CONSOLE	1
#define	NEWLINE	'\n'
#define	RETURN	'\r'

int	slowtime = 1000;

/*-----------------------------------------------------------------------------
 *  kgetc  --  read a character from the CONSOLE
 *-----------------------------------------------------------------------------
 */

char	
kgetc(device)
	int	device;
{
	register struct	uart	*uart;
	int	slowdown;

	uart = (struct uart *)(device == CONSOLE ? 0x600000 : 0x600004);
	while ((uart->u_status & NUFULL) == 0);
	return(uart->u_data);
}

/*-----------------------------------------------------------------------------
 *  kputc  --  write a character on the console using polled I/O
 *-----------------------------------------------------------------------------
 */

int	kputc(device, c)
	int	device;	
	register char c;
{
	register struct	uart	*uart;
	int	slowdown;

	uart = (struct uart *)(device == CONSOLE ? 0x600000 : 0x600004);
	if ( c == 0 )
		return;
	if ( c == NEWLINE )
		kputc(device, RETURN);
	while ((uart->u_status & NUEMPTY) == 0);
	uart->u_data = c;
	for(slowdown=0; slowdown < slowtime; slowdown++);
	return(OK);
}

/*-----------------------------------------------------------------------------
 *  kprintf  --  kernel printf: formatted, unbuffered output to CONSOLE
 *-----------------------------------------------------------------------------
 */

int	kprintf(fmt, args)
	char *fmt;
{
	int	kputc();

	_doprnt(fmt, &args, kputc, CONSOLE);
	return(OK);
}
