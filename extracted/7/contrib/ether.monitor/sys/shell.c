/* mshell.c - mshell */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <sleep.h>
#include <slu.h>
#include <proc.h>
#include <deqna.h>
#include <ether.h>
#include <ip.h>
#include <arp.h>
#include <ctype.h>
#include "../h/tcp.h"
#include "../h/network.h"
#include "../h/mshell.h"
#include "../h/mshellc.h"
#include "../h/monitor.h"

Bool	found;
Bool	match;
struct	epacket	*epptr;
struct	name	table[] = {TABLE};
struct	mcmdent	mcmds[] = {CMDS};	/* shell commands (mshellc.h)	*/
struct	mshvars	Msh;			/* globals used by pseudo shell	*/
LOCAL	char	errhd[] = "Syntax error\n";
LOCAL	char	fmt[]   = "Cannot open %s\n";


struct	name	*lookup(list, nam)
	struct	name	*list;
	char	*nam;
	{
		while(!blkequ(list->n_nam, ZERO, strlen(list->n_nam))){
			if(blkequ(nam, list->n_nam, strlen(list->n_nam))){
				return list;
			}else
				list++;
		}
		return(SYSERR);
	}

struct	sign	*checkname(list, name)
	register struct	sign	*list;
	char	*name;
	{

		while(list->s_name){
			if(blkequ(name, list->s_name, strlen(list->s_name))){
				return(list);
			}
			list++;
		}
		return(0);
	}

struct	sign	*checknum(list, num)
	register struct	sign	*list;
	unsigned	*num;
	{
		while(list->s_name){
			if(list->s_number == num){
				return(list);
			}
			list++;
		}
		return(list);
	}

/*------------------------------------------------------------------------
 *  mshell  -  pseudo shell with a few hard-wired commands
 *------------------------------------------------------------------------
 */
main(dev)
int	dev;
{
	int	ntokens;
	int	i, j, k, len, length, num;
	int	com, recv, pool;
	int	stdin, stdout, stderr;
	Bool    mark;
	Bool	flag;
	char	ch, ps, **pa;
	int	child;
	struct	epacket	*getbuf();
	struct	dcmd	ercmd[2];

	mtabinit(table);
	minit();
	Msh.mncmds = sizeof(mcmds)/sizeof(struct mcmdent);
	stdin = stdout = stderr = dev = 0;
	pool = mkpool(EMAXPAK,1);
	while (TRUE) {
		sleep(1);
		fprintf(dev,"Enter command ## ");
		if((len = read(dev,Msh.mshbuf,128)) ==0){
			len = read(dev, Msh.mshbuf, 128);
		}
		if (len != 0)
		if (len == EOF)
			break;
		Msh.mshbuf[len-1] = NULLCH;

		control(dev, TCINT, getpid());

		for(k=0, match=FALSE, found=FALSE; k<500 && found==FALSE; k++){
			fprintf(dev,"\n",k);
			fprintf(dev,"Packet %u\n",k);
			if ( (ntokens=mparse(Msh.mshbuf)) == SYSERR) {
				fprintf(dev, errhd);
				continue;
			} else if (ntokens == 0){
				continue;
			}
			epptr = getbuf(pool);
			while((length = eread(ercmd,epptr,sizeof(*epptr)))==                                 SYSERR){
				continue;
			}

			/* scan tokens, handling '|'  */

			do {

				flag = FALSE;
				mark = FALSE;
				for (len=0, i=0 ; i<ntokens ;) {
					if((ch=Msh.mtoktyp[i]) == '|'){
						if(Msh.mtoktyp[i+1] == 'D' ||    Msh.mtoktyp[i+1] == 'd' || Msh.mtoktyp[i+1] == 'S' || Msh.mtoktyp[i+1] == 's'){
							mark = TRUE;
						}else{
							mark = FALSE;
						}
						flag = TRUE;
						num = ntokens-i;
						ntokens = i;
						break;
					}else {
						len += strlen(Msh.mtokens[i++]);
						continue;
					}
				}
				for(com=0; com<Msh.mncmds; com++){
					if(strcmp(mcmds[com].cmdnam,Msh.mtokens[0])==0)
						break;
				}
				if (com >= Msh.mncmds){
					fprintf(dev,"%s: not found\n",Msh.mtokens[0]);
					found = TRUE;
					freebuf(epptr);
					break;
				}

				len += (ntokens+2)*(sizeof(int)+sizeof(char));
				if(isodd(len))
					len--;

				/* create process to execute command */

				if((child=create(mcmds[com].cproc,500,19,Msh.mtokens[0],                          (len/sizeof(int))+4,stdin,stdout,stderr,ntokens))                               == SYSERR){
					fprintf(dev,"Cannot create\n");
					freebuf(epptr);
					break;
				}
				maddarg(child,ntokens,len);
				setnok(getpid(),child);
				recvclr();
				resume(child);
				if(receive() == INTRMSG){
					setnok(BADPID, child);
					flag = FALSE;
					found = FALSE;
				}
				if(found == TRUE)
					break;
				if (flag == FALSE){
					break;
				}
				ntokens = num;
				if(mark){
					for(j=1; j<ntokens; j++){
						Msh.mtoktyp[j]=Msh.mtoktyp[j+i];
						Msh.mtokens[j]=Msh.mtokens[j+i];
					}
				}else{
					for(j=0; j<(ntokens-1); j++){
						Msh.mtoktyp[j]=Msh.mtoktyp[j+i+1];
						Msh.mtokens[j]=Msh.mtokens[j+i+1];
					}
					ntokens -= 1;
				}
			}while(flag == TRUE);
			freebuf(epptr);
		}
		if(k>=500 && match==FALSE){
			fprintf(dev,"Now! %d packets have arrived, but no address matches !\n", k);
		}
	}
	return;
}
