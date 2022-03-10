/* mshell.c - mshell */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <slu.h>
#include <proc.h>
#include <deqna.h>
#include <ether.h>
#include <ctype.h>
#include "../h/tcp.h"
#include "../h/network.h"
#include "../h/mshell.h"
#include "../h/mshellc.h"
#include "../h/monitor.h"

Bool	found;
Bool	match;
int	or;
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
		int	len1, len2;

		len1 = strlen(nam);
		while(!blkequ(list->n_nam, ZERO, strlen(list->n_nam))){
			len2 = strlen(list->n_nam);
			if(blkequ(nam, list->n_nam, ((len1 >= len2)? len1 : len2))){
				return list;
			}else
				list++;
		}
		return(SYSERR);
	}

struct	name	*lookaddr(list, addr)
	struct	name	*list;
	char	*addr;
	{
		int	len1, len2;

		len1 = strlen(addr);
		while(!blkequ(list->n_nam, ZERO, strlen(list->n_nam))){
			len2 = strlen(list->n_ipaddr);
			if(blkequ(addr, list->n_ipaddr, ((len1 >= len2)? len1 : len2))){
				return list;
			}else
				list++;
		}
		return(list);
	}

struct	sign	*checkname(list, name)
	register struct	sign	*list;
	char	*name;
	{

		int	len1, len2;
		len1 = strlen(name);
		while(list->s_name){
			len2 = strlen(list->s_name);
			if(blkequ(name, list->s_name, ((len1 >= len2)? len1 : len2))){
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
	Bool	flag;
	char	ch;
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
		if (len == EOF)
			break;
		Msh.mshbuf[len-1] = NULLCH;

		control(dev, TCINT, getpid());

		for(k=0, match=FALSE, found=FALSE; k<500 && found==FALSE; k++){
			fprintf(dev,"\n",k);
			fprintf(dev,"Packet %d\n",k);
			if ( (ntokens=mparse(Msh.mshbuf)) == SYSERR) {
				fprintf(dev, errhd);
				continue;
			} else if (ntokens == 0){
				continue;
			}
			epptr = getbuf(pool);
			while((length = meread(ETHER,ercmd,epptr,sizeof(*epptr)))==                                 SYSERR){
				continue;
			}

			/* scan tokens, handling '|'  */

			do {

				flag = FALSE;
				or = 0;
				for (len=0, i=0 ; i<ntokens ;) {
					if((ch=Msh.mtoktyp[i]) == '|'){
						if(Msh.mtoktyp[i+1] != 'D' &&    Msh.mtoktyp[i+1] != 'd' && Msh.mtoktyp[i+1] != 'S' && Msh.mtoktyp[i+1] != 's'){
							flag = TRUE;
							num = ntokens-i;
							ntokens = i;
							break;
						}else {
							or++;
							for(ntokens--,j=i;                                                                  j<ntokens;j++){
								Msh.mtoktyp[j]=                                                                Msh.mtoktyp[j+1];
								Msh.mtokens[j]=                             				       Msh.mtokens[j+1];
							}
							continue;
						}
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

				if((child=create(mcmds[com].cproc,600,19,Msh.mtokens[0],                          (len/sizeof(int))+4,stdin,stdout,stderr,ntokens))                               == SYSERR){
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
					found = TRUE;
				}
				if(found == TRUE)
					break;
				if (flag == FALSE){
					break;
				}
				ntokens = num;
				for(j=0; j<(ntokens-1); j++){
					Msh.mtoktyp[j]=Msh.mtoktyp[j+i+1];
					Msh.mtokens[j]=Msh.mtokens[j+i+1];
				}
				ntokens -= 1;
			}while(flag == TRUE);
			freebuf(epptr);
		}
		if(k>=500 && match==FALSE){
			fprintf(dev,"Now! %d packets have arrived, but no address matches !\n", k);
		}
	}
	return;
}
/* maddarg.c - maddarg */


/*------------------------------------------------------------------------
 *  maddarg  -  copy shell arguments to area reserved on process stack
 *------------------------------------------------------------------------
 */
maddarg(pid, nargs, len)
int	pid;				/* process to receive arguments	*/
int	nargs;				/* number of arguments to copy	*/
int	len;				/* size of arg. area (in bytes)	*/
{
	struct	pentry	*pptr;
	int	*fromarg;
	int	*toarg;
	char	*to;

	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate != PRSUSP)
		return(SYSERR);
	toarg = (int *) ( ((unsigned)pptr->pbase) - (unsigned)len );
	to = (char *) (toarg + (nargs + 2));
	*toarg++ = toarg + 1;
	for (fromarg= &Msh.mtokens ; nargs > 0 ; nargs--) {
		*toarg++ = to;
		strcpy(to, *fromarg++);
		to += strlen(to) + 1;		
	}
	*toarg = 0;
	return;
}
/* mparse.c - mparse */


/*------------------------------------------------------------------------
 *  mparse  -  simplistic, ad hoc parser for command line
 *------------------------------------------------------------------------
 */
mparse(line)
char	*line;
{
	char	**tokptr;
	int	ntok, i;
	char	*p;
	char	ch;
	char	*to;

	to = &Msh.margstr;
	tokptr = &Msh.mtokens[ntok = 0];
	for (p=line; *p!='\n' && *p!='\0' && ntok<MMAXTOK; ){
		while ( (ch = *p) == ' ')	/* skip leading blanks	*/
			p++;
		if (ch=='\0' || ch=='\n')      /* end of line or string */
			return(ntok);
		*tokptr++ = to;
		Msh.mtoktyp[ntok++] = ch;
		*to++ = *p++;
		if (ch != '|'){
			while ((ch = *p) != '\n' && ch != '\0'                                                  && ch != '|' && ch != ' ')
				*to++ = *p++ ;
		}
		*to++ = NULLCH;
	}
	return(ntok);
}
