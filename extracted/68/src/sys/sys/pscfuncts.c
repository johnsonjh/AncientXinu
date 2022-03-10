#include <kernel.h>
#include <proc.h>
disable()
{
#ifdef	DEBUG
	kprintf("disable()\n");
#endif
	if (!(proctab[currpid].phasps)++)
		_disable(&(proctab[currpid].pstatreg));
}

restore()
{
#ifdef	DEBUG
	kprintf("restore()\n");
#endif
	if (--(proctab[currpid].phasps) == 0)
		_restore(&(proctab[currpid].pstatreg));
}
