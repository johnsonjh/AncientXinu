#
/* PDP-11 Assembler for the VAX Pass 1 */

#include <stdio.h>
#include <signal.h>
#include "vars2.h"

char *sbrk();
int outmod = 0777;


main(argc,argv) char **argv; {
	int (*savint)();
	int saexit();
	int chr;
	extern int end;

	savint = signal(SIGINT, SIG_IGN);
	if(((int)savint & 1) == 0) signal(SIGINT, saexit);
	argv++;
	argc--;
	while(argv[0][0] == '-') {
		chr = argv[0][1];
		if(chr == 'g') defund = 040;
		else if(chr == 'o') {
			a_outp = *++argv;
			argc--;
		}
		argv++;
		argc--;
	}
	if(argc != 3) aexit();
	a_tmp1 = *argv++;
	a_tmp2 = *argv++;
	a_tmp3 = *argv++;
	if((fout = creat(a_outp, 0666)) < 0) filerr(a_outp);
	memptr = (short *)&end;
	endcore = sbrk(0);
	go();
}

go() {
	int temp, temp1, i;
	short *iptr;
	struct symbol *sptr;
	extern int end;

	ofile(a_tmp3);
	usymtab = (struct symbol *)memptr;
	for(; (temp = getshort()) != -1;) {
		SYMSIZ += 014;
		temp = getshort();
		temp = getshort();
		temp = getshort();
		temp = getshort();
		setbrk();
		if( (temp1 = temp & 037) == 02 || temp1 == 03) {
			*memptr++ = temp1 + 031;
			*memptr++ = getshort();
		} else {
			temp = getshort();
			*memptr++ = 0;
			*memptr++ = 0;
		}
	}
	fbbufp = (struct symbol *)memptr;
	ofile(a_tmp2);
	for(; (temp = getshort()) != -1;) {
		setbrk();
		*memptr++ = temp + 031;
		*memptr++ = getshort();
	}
	endtable = (struct symbol *)memptr;
	*memptr++ = 0100000;
	setup();
	assem();
	if(outmod != 0777) aexit();
	DOT = 0;
	DOTREL = 2;
	DOTDOT = 0;
	brtabp = 0;
	setup();
	passno++;
	BSSSIZ = (BSSSIZ + 1) & ~1;
	TXTSIZ = (TXTSIZ + 1) & ~1;
	DATSIZ = (DATSIZ + 1) & ~1;
	savdot[1] = datbase = TXTSIZ;
	savdot[2] = bssbase = datbase + DATSIZ;
	symseek = ((long)bssbase << 1) + 020;
	relseeks[1] = symseek - DATSIZ;
	relseeks[0] = relseeks[1] - TXTSIZ;
	dataseeks[1] = relseeks[0] - DATSIZ;
	for(sptr = usymtab; sptr < endtable; sptr++) doreloc(sptr);
	oset(&txtp, 0L);
	oset(&relp, relseeks[0]);
	for(i = 8, iptr = (short *)&txtmagic; i > 0; i--)
		putw(*iptr++, &txtp);
	assem();
	flush(&txtp);
	flush(&relp);
	ofile(a_tmp3);
	oset(&txtp, symseek);
	for(iptr = (short *)usymtab; (i = getshort()) != -1; ) {
		putw(i, &txtp);
		putw(getshort(), &txtp);
		putw(getshort(), &txtp);
		putw(getshort(), &txtp);
		putw((int)*iptr++, &txtp);
		putw((int)*iptr++, &txtp);
		getshort();
		getshort();
	}
	flush(&txtp);
	aexit();
}

saexit() {
	errflg = 1;
	aexit();
}

aexit() {
	int mask;

	unlink(a_tmp1);
	unlink(a_tmp2);
	unlink(a_tmp3);
	if(!errflg) {
		mask = umask(0);
		outmod &= ~mask;
		chmod(a_outp, outmod);
		exit(0);
	}
	exit(2);
}

filerr(name) char *name; {
	fprintf(stderr, "%s?\n", name);
	aexit();
}

doreloc(sptr) register struct symbol *sptr; {
	int temp;

	if((temp = sptr->s_type) == 0) sptr->s_type |= defund;
	temp &= 037;
	if(temp == 04) sptr->s_value += bssbase;
	else if(temp == 03) sptr->s_value += datbase;
}

setbrk() {
	if((char *)memptr + 020 > endcore)
		endcore = sbrk(512) + 512;
}

setup() {
	int i;

	for(i = 0; i < 10; i++) {
		curfb[i] = (struct symbol *)0;
		nxtfb[i] = (struct symbol *)0;
	}
	ofile(a_tmp1);
	for(i = 0; i < 10; i++)
		fbadv(i);
}

ofile(name) char *name; {

	if(freopen(name, "r", stdin) == NULL) filerr(name);
}

