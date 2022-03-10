/* mshell.c - mshell */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <proc.h>
#include <deqna.h>
#include <ether.h>
#include <ip.h>
#include "../h/network.h"
#include "../h/mshell.h"
#include "../h/mshellc.h"
#include <sleep.h>
#include <slu.h>

#define CDEVICE		0177560
#define	kputc(x,c)	while((x->ctstat & SLUREADY) == 0); x->ctbuf = c
Bool	found;
char	str[80];
struct	epacket	*packet;
struct	mshvars	Msh;			/* globals used by pseudo shell	*/
struct	mcmdent	mcmds[] = {CMDS};	/* shell commands (mshellc.h)	*/
LOCAL	char	errhd[] = "Syntax error\n";
LOCAL	char	fmt[]   = "Cannot open %s\n";
LOCAL	char	fmt2[]  = "[%d]\n";


/*------------------------------------------------------------------------
 *  mshell  -  pseudo shell with a few hard-wired commands
 *------------------------------------------------------------------------
 */
main(dev)
int	dev;
{
	int	ntokens;
	int	*saddr;
	int	i, j, k, len, length, num;
	int	com, pool;
	int	stdin, stdout, stderr;
	Bool    mark;
	Bool	flag;
	char	ch, mach[12], ps;
	int	child;
	struct	epacket	*getbuf();
	struct	ip	*ipptr;
	struct	dcmd	ercmd[2];

	minit();
	Msh.mncmds = sizeof(mcmds)/sizeof(struct mcmdent);
	stdin = stdout = stderr = dev = 0;
	pool = mkpool(EMAXPAK,5);
	while (TRUE) {
		fprintf(dev,"## ");
		if((len = read(dev,Msh.mshbuf,128)) ==0){
			len = read(dev, Msh.mshbuf, 128);
		}
		if (len != 0)
			fprintf(dev,"len=%d\n",len);
		if (len == EOF)
			break;
		Msh.mshbuf[len-1] = NULLCH;
		for(k=0, found=TRUE; k<20 && found==TRUE; k++){
			fprintf(dev,"k = %d\n",k);
			if ( (ntokens=mparse(Msh.mshbuf)) == SYSERR) {
				fprintf(dev, errhd);
				continue;
			} else if (ntokens == 0){
				fprintf(dev,"Ntokens == 0!\n");
				continue;
			}
			packet = getbuf(pool);
			if(packet == SYSERR){
				fprintf(dev,"Buffer is full!\n");
			}
			fprintf(dev,"Getbuf has been completed!\n");
			while((length = eread(ercmd,packet,sizeof(*packet)))==                                 SYSERR){
				fprintf(dev,"Eread error!\n");
				continue;
			}
			fprintf(dev,"Eread is over!\n");
			switch(net2hs(packet->e_ptype)){
				case EP_IP:
					ipptr = (struct ip*)packet->ep_data;
					sprintf(str,"%d.%d.%d.%d -> %d.%d.%d.%d\n",ipptr->i_src[0]&0377,ipptr->i_src[1]&0377,ipptr->i_src[2]&0377,ipptr->i_src[3]&0377,ipptr->i_dest[0]&0377,ipptr->i_dest[1]&0377,ipptr->i_dest[2]&0377,ipptr->i_dest[3]&0377);
					write(dev,str,strlen(str));
					break;
				default:
					break;
			}

		/* scan tokens, handling '|' and '&' */

		do {

			flag = FALSE;
			mark = FALSE;
			for (len=0, i=0 ; i<ntokens ;) {
				if((ch=Msh.mtoktyp[i]) == '|'){
					if(Msh.mtoktyp[i+1] == 'f' ||                                                      Msh.mtoktyp[i+1] == 't'){
						mark = TRUE;
					}else{
						mark = FALSE;
					}
					flag = TRUE;
					num = ntokens-i;
					ntokens = i;
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
				found = FALSE;
				freebuf(packet);
				break;
			}

			len += (ntokens+2)*(sizeof(int)+sizeof(char));
			if(isodd(len))
				len--;

			/* create process to execute command */

			if((child=create(mcmds[com].cproc,500,20,Msh.mtokens[0],                          (len/sizeof(int))+4,stdin,stdout,stderr,ntokens))                               == SYSERR){
				fprintf(dev,"Cannot create\n");
				break;
			}
			fprintf(dev,"Continue\n");
			maddarg(child,ntokens,len);
			setnok(getpid(),child);
			recvclr();
			resume(child);
			while(receive() != child){
				fprintf(dev,"Wait!\n");
				continue;
			}
			setnok(BADPID,child);
			fprintf(dev,"Resume!\n");
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
				for(j=0; j<ntokens; j++){
					Msh.mtoktyp[j]=Msh.mtoktyp[j+i+1];
					Msh.mtokens[j]=Msh.mtokens[j+i+1];
				}
				ntokens -= 1;
			}
			fprintf(dev,"%d\n",flag);
		}while(flag == TRUE);
	freebuf(packet);
	}
	}
	return;
}
