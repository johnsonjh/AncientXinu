#include "../h/conf.h"
#include "../h/kernel.h"
#include <proc.h>
#include "../h/mshellc.h"



int	micmp();
struct mcmdent mcmds[] = {
	{"icmp",micmp},
	{0,0}
};
	main()
	{
		int	pid;

		if ((pid=newpid())==SYSERR){
			printf("Slot is not free\n");
		}
		printf("hello.\n");
		micmp();
	}
	newpid()
	{
		int	pid;
		int	i;

		for(i=0;i<NPROC;i++){
			if((pid=nextproc--)<=0)
				nextproc=NPROC-1;
			if(proctab[pid].pstate==PRFREE)
				return(pid);
		}
		return(SYSERR);
	}
micmp()
{
	printf("Ok\n");
}
