/* nserver.c - nserver */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <network.h>
#include <serve11.h>


/*------------------------------------------------------------------------
 *  nserver  -  provide name server for the network
 *------------------------------------------------------------------------
 */
PROCESS nserver()
{
	Bool	done;
	struct	dn_mesg *dnptr;
	int	devid, i, j, nmx, size, maxpacket;
	IPaddr	ipaddr;
	char	*msg, w;
	char	*p, *q, *tmp, *beg, *end;

	if ( (devid=open(INTERNET, ANYFPORT, UDNAME)) == SYSERR) {
		kprintf("\nName Serve - cannot open.\n");
		return(SYSERR);
		}
	maxpacket = XGHLEN + DN_MLEN;
	if ((msg = (char *) getmem(maxpacket)) == SYSERR ||
	    (tmp = (char *) getmem(NETNLEN))   == SYSERR) {
		panic("nserver: out of memory");
		return(SYSERR);
		}
	while ( TRUE ) {
		if ((size = read(devid, msg, maxpacket)) == SYSERR) {
			continue;
			} 
		dnptr = (struct dn_mesg *) ((struct xgram *)msg)->xg_data;
		if ((dnptr->dn_opparm & hs2net(DN_RD)) != hs2net(DN_RD) ||
		   dnptr->dn_qcount != hs2net(1)     ||
		   dnptr->dn_qaaa[0] == NULLCH ) {
			continue;
			} 
		p = dnptr->dn_qaaa;
		i = IPLEN - 1;
		while (*p != NULLCH) {
			if (*p & DN_CMPRS) {
				p = dnptr + (net2hs(*(int *)p) & DN_CPTR);
				} 
			else {
				if (i >= 0) {
					blkcopy(tmp, p+1, (int)(*p));
					tmp[(int)(*p)] = NULLCH;
					sscanf(tmp, "%d", &w);
					ipaddr[i--] = LOWBYTE & w;
					} 
				else {
					*tmp = NULLCH;
					strncat(tmp, p+1, (int)(*p));
					} 
				p += *p + 1;
				} 
			} 
		p += sizeof(struct dn_qsuf) + 1;

		for (nmx=0; nmx<mntabsize; nmx++) 
			if (blkequ(mnametab[nmx].ipaddr, ipaddr, IPLEN))
				break;

		if (nmx < mntabsize) {
			/* repeat query here */
			for (i=IPLEN-1; i >= 0; i--) {
				sprintf(tmp, "%d", (int)mnametab[nmx].ipaddr[i] & LOWBYTE);
				dn_cat(p, tmp);
				} 
			dn_cat(p, "in-addr");
			dn_cat(p, "arpa");
			*p++ = NULLCH;

			*((short *)(p)) = hs2net(DN_QTPR);	/* type */
			p += 2;
			*((short *)(p)) = hs2net(DN_QCIN);	/* class */
			p += 2;
			*((long *)(p))  = (long)0;		/* ttl */
			p += 4;
			/* length = # of name sections + # chars (not '.'s) in name + 1 for the trailing NULLCH */
			*((short *)(p)) = hs2net(strlen(mnametab[nmx].name) + 2);	/* length */
			p += 2;

			strcpy(tmp,  mnametab[nmx].name);
			beg = tmp;
			end = index(beg, '.');
			done = FALSE;
			while (!done) {
 				if (end)
					*end = NULLCH;
				else
					done = TRUE;
				dn_cat(p, beg);
				if (!done) {
					beg = end + 1;
					end = index(beg, '.');
					} 
				} 
			*p++ = NULLCH;
			((struct dn_qsuf *)p)->dn_type = hs2net(DN_QTHI);
			((struct dn_qsuf *)p)->dn_clas = hs2net(DN_QCIN);
			p += sizeof(struct dn_qsuf);

			dnptr->dn_opparm = hs2net(DN_AA);
			dnptr->dn_acount = hs2net(1);

			if (write(devid, msg, p-dnptr) == SYSERR) {
                   		panic("nserver -- cannot write");
                   		}
			} 
		else {
			dnptr->dn_opparm = hs2net(3);
			
			if (write(devid, msg, p-dnptr) == SYSERR) {
                   		panic("nserver -- cannot write");
                   		}
			}
		}
}
