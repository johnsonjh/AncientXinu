/* prdump.c - prdump, prdumph, prdumpa */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

/*------------------------------------------------------------------------
 *  prdump  --  dump the active process table entries
 *------------------------------------------------------------------------
 */
prdump()
{
	pxdump(0);
}

/*------------------------------------------------------------------------
 *  prdumph  --  dump the active process table entries and halt
 *------------------------------------------------------------------------
 */
prdumph()
{
	pxdump(0);
	kprintf("\nDump complete -- type P to continue\n");
	halt();
}

/*------------------------------------------------------------------------
 *  prdumpa  --  dump the process table entries printing all entries
 *------------------------------------------------------------------------
 */
prdumpa()
{
	pxdump(1);
}
static pxdump(all)
int all;
{
        int i, j, ctr;
        char ps;
        struct pentry *pptr;
        disable(ps);
        kprintf("\nProctab at loc %o\ncurrpid is %d\n", proctab, currpid);
	for ( i=0 ; i<NPROC ; i++ ) {
            pptr = &proctab[i];
	    if (all!=0 || pptr->pstate!=PRFREE) {
                kprintf("\nProcess %d: ",i);
                kprintf("state=%o,name=",pptr->pstate);
                for ( j=0 ; j<PNMLEN ; j++ ) {
                        if (pptr->pname[j]=='\0') break;
                        kprintf("%c",pptr->pname[j]);
                }
                kprintf(",prio=%d,sem=%d,stk:base=%o,limit=%o,",
			pptr->pprio,pptr->psem,pptr->pbase,pptr->plimit);
		kprintf("MAGIC=%o\n", *((int *)pptr->pbase) );
                for ( j=0 ; j<PNREGS ; j++ )
                        kprintf("(R%c)%o ","01234567S"[j],pptr->pregs[j]);
                kprintf("\n");
	    }
	}
	kprintf("rdyhead:");
	ctr = NPROC;
        for ( i = q[rdyhead].qnext; i < NPROC ; i=q[i].qnext) {
                kprintf("%d ",i);
		if (q[i].qnext == i) {
			kprintf("\nLOOP in ready list");
			break;
		}
		if (--ctr < 0) {
			kprintf("\nCorrupt ready list -- too long");
			break;
		}
	}
        kprintf("\n");
        restore(ps);
}
