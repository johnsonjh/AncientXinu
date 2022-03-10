/* x_snap.c - x_snap */

#include <conf.h>
#include <kernel.h>
#include <core11.h>
#include <mem.h>

LOCAL	int	snapreg;
#define	SNAPSIZ	512

/*------------------------------------------------------------------------
 *  x_snap  -  (command snap) write snapshot of memory to a core file
 *------------------------------------------------------------------------
 */
COMMAND	x_snap(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	dev;
	struct	core11	hdr;
	char	*p, *limit;
	unsigned len;

	if (nargs > 2) {
		fprintf(stderr, "use: snap [core-file]\n");
		return(SYSERR);
	}
	p = nargs==1 ? "core11" : args[1];
	if ((dev=open(NAMESPACE, p, "w")) == SYSERR) {
		fprintf(stderr, "snap: cannot write %s\n", p);
		return(SYSERR);
	}

	/* make up a core image using core11 structure heading format */

	hdr.c_magic = COREMAGIC;
	hdr.c_size = hdr.c_zero1 = hdr.c_zero2 = hdr.c_zero3 =
		hdr.c_zero4 = hdr.c_zero5 = 0;

	/* Capture machine registers */

	asm("mov r0,_snapreg"); hdr.c_regs[0] = snapreg;
	asm("mov r1,_snapreg"); hdr.c_regs[1] = snapreg;
	asm("mov r2,_snapreg"); hdr.c_regs[2] = snapreg;
	asm("mov r3,_snapreg"); hdr.c_regs[3] = snapreg;
	asm("mov r4,_snapreg"); hdr.c_regs[4] = snapreg;
	asm("mov r5,_snapreg"); hdr.c_regs[5] = snapreg;
	asm("mov sp,_snapreg"); hdr.c_regs[6] = snapreg;
	asm("mov pc,_snapreg"); hdr.c_regs[7] = snapreg;
	asm("clr _snapreg;mfps _snapreg"); hdr.c_psw = snapreg;
	fprintf(stderr, "Writing core image");
	write(dev, &hdr, sizeof(struct core11));

	/* Add contents of real memory to core image */

	limit = (char *) ( 1 + (unsigned)maxaddr );
	for (p=NULL ; p <= limit ; p += SNAPSIZ) {
		putc(stderr, '.');
		len = 1 + ( (unsigned)limit - (unsigned)p );
		if ( len > (unsigned) SNAPSIZ )
			len = SNAPSIZ;
		write(dev, p, len);
	}
	putc(stderr, '\n');
	close(dev);
	return(OK);
}
