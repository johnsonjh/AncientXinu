/* serinit.c - serinit */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <date.h>
#include <ctype.h>
#include <network.h>
#include <serve11.h>

/*------------------------------------------------------------------------
 *  serinit - ask user what to run and record in server info structure
 *------------------------------------------------------------------------
 */
serinit()
{
	char	*line, *name, *tmp;
	int	i, j, n;
	int	yy, mm, dd, hh, mn, ss;
	int	year, month; 
	long	clk;
	long	days;
	Bool	datefnd, done;
	int	a1, a2, a3, a4;
	int	rserver();

	if ((line = (char *) getmem(80)) == SYSERR ||
	    (tmp  = (char *) getmem(80))   == SYSERR ||
	    (name = (char *) getmem(NETNLEN))   == SYSERR) {
		panic("serinit: out of memory");
		return(SYSERR);
		}
	/* ask about RARP, IP network address, Time & Date, file server
		domain name server */
	kprintf("\nserver initialization --\n\n");
	serinfo.ser_rcnt = serinfo.ser_tcnt =
		serinfo.ser_ncnt = serinfo.ser_fcnt = 0;
	serinfo.ser_rarp = serinfo.ser_time = serinfo.ser_name
		= serinfo.ser_file = FALSE;
        Net.mavalid = FALSE;
	kprintf("Type y to provide RARP server\n: ");
	if ( (n=read(CONSOLE, line, 80)) > 1 && line[0] == 'y') {
		kprintf("Setting up to run RARP server\n");
		resume( Arp.rarpid
			= create(rserver, 400, 50, "rserver", 0) );
		serinfo.ser_rarp = TRUE;
		for (n=0 ; n<=0 ; ) {
		    kprintf("%s\n%s\n: ",
		    "Type this machine's IP address in dotted decimal ",
		    "  notation or carriage return to use 128.10.2.3");
		    if ( (n=read(CONSOLE, line, 80)) <= 0)
			continue;
		    Net.mavalid = TRUE;
		    if (n == 1) {
			dot2ip(Net.myaddr, 128, 10, 2, 3);
			netnum(Net.mynet, Net.myaddr);
			break;
		    }
		    line[n] = NULLCH;
		    sscanf(line, "%d.%d.%d.%d", &a1, &a2, &a3, &a4);
		    kprintf("Using %d.%d.%d.%d\n", a1, a2, a3, a4);
		    dot2ip(Net.myaddr, a1, a2, a3, a4);
		    netnum(Net.mynet, Net.myaddr);
		}
	}
	kprintf("Type y to provide time server\n: ");
	if ( (n=read(CONSOLE, line, 80)) > 1 && line[0] == 'y') {
		kprintf("Setting up to run time server\n");
		serinfo.ser_time = TRUE;
		for(datefnd=FALSE ; !datefnd ; ) {
			if (clktime < SECPERHR)
				clktime = 365 * SECPERDY;
			ascdate(ut2ltim(clktime), line);
			kprintf("Date is now set to   %s\n", line);
			kprintf("%s\n%s\n: ",
			    "Type carriage return to accept or",
			    "enter current date as yymmddhhmmss");
			if ( (n=read(CONSOLE, line, 80)) == 1)
				break;
			else if (n != 13)
				continue;
			for (i=0 ; i<12 ; i++)
				if (!isdigit(line[i]))
					continue;
			sscanf(line,"%2d%2d%2d%2d%2d%2d",&yy, &mm, &dd,
				&hh, &mn, &ss);
			if (yy<86 || mm==0 || mm>12 || 
			    dd == 0 || dd>Dat.dt_msize[mm-1] ||
			    hh>23 || mn>59 || ss>59)
				continue;
			mm--;
			dd--;
			yy += 1900;
			clk = 0L;
			for (year=1970 ; year < yy ; year++) {
				days = isleap(year) ? 366 : 365;
				clk += days * SECPERDY;
			}
			for (month=0 ; month < mm ; month++)
				clk += SECPERDY * Dat.dt_msize[month];
			clk += SECPERDY * dd;
			clk += SECPERHR * hh;
			clk += SECPERMN * mn;
			clk += ss;
			clktime = ltim2ut(clk);
		}
	}
	kprintf("Type y to provide name server\n: ");
	if ( (n=read(CONSOLE, line, 80)) > 1 && line[0] == 'y') {
		kprintf("Setting up to run name server\n");
		mntabsize = 0;
		serinfo.ser_name = TRUE;
		strcpy(name, MYNAME);
		done = FALSE;
		while (!done) {
			kprintf("%s\n%s%s\n:  ",
			    "Type this machine's name or", 
			    "carriage return to use ", name);
			if ((n=read(CONSOLE, line, NETNLEN)) > 1) {
				line[n] = NULLCH;
				sscanf(line, "%s", name);
				continue;
				}
			else 
				done = TRUE;
			}
		strcpy(Net.myname, name);
		Net.mnvalid = TRUE;
		if (Net.mavalid) {
			blkcopy(mnametab[0].ipaddr, Net.myaddr, IPLEN );
			strcpy(mnametab[0].name, Net.myname);
			i = 1;
			}
		else 
			i = 0;
		while (i < MNTABSIZE) {
			kprintf("%s\n%s\n:  ",
			    "Type in an IP address in dotted decimal form and",
			    "a machine name or carriage return to quit");
			if ((n=read(CONSOLE, line, 80)) == 1) 
				break;
			line[n] = NULLCH;
			sscanf(line, "%d.%d.%d.%d %s",&a1, &a2, &a3, &a4, tmp);
			if (strlen(tmp) <= NETNLEN-1)
				strcpy(name, tmp);
			else
				strncpy(name, tmp, NETNLEN-1);
			kprintf("%s\n(%d.%d.%d.%d, %s)%s\n:  ",
			    "Is the IP-address / machine name pair ", a1, 
			    a2, a3, a4, name, " correct? (y/n)");
			if ((n=read(CONSOLE, line, 80)) >= 1 && 
			    line[0] == 'y') {
				dot2ip(mnametab[i].ipaddr, a1, a2, a3, a4);
				strcpy(mnametab[i].name, name);
				i++;
				kprintf("%s(%d.%d.%d.%d, %s)%s\n\n",
				    "Putting ", a1, a2, a3, a4, name,
				    " in name table");
				}
			}
		mntabsize = i;
		kprintf("The name table:\n\n");
		for(j=0; j<i; j++) 
			kprintf("  %d.%d.%d.%d		%s\n",
			    (int)mnametab[j].ipaddr[0] & LOWBYTE,
			    (int)mnametab[j].ipaddr[1] & LOWBYTE,
			    (int)mnametab[j].ipaddr[2] & LOWBYTE,
			    (int)mnametab[j].ipaddr[3] & LOWBYTE,
			    mnametab[j].name);
		kprintf("\n");
	}
	kprintf("Type y to provide file server\n: ");
	if ( (n=read(CONSOLE, line, 80)) > 1 && line[0] == 'y') {
		kprintf("Setting up to run file server\n");
		serinfo.ser_file = TRUE;
	}
	freemem(line, 80);
	freemem(tmp, 80);
	freemem(name, NETNLEN);
}

int mntabsize;
struct	mnament mnametab[MNTABSIZE];
struct	serv	serinfo;

