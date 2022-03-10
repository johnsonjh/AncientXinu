/* PDP-11 Assembler for the VAX Pass 1 */

#include <stdio.h>
#include "vars2.h"
#include "stypes.h"

outw(flag, val) short flag, val; {
	short temp;

	if(DOTREL == TBSS) {
		error('x');
		return;
	}
	if(DOT & 1) {
		error('o');
		outb(0, val);
		return;
	}
	DOT += 2;
	if(passno == 0) return;
	temp = (flag & 0100000) ? 1 : 0;
	flag &= ~0100000;
	if(flag == TGLOBAL) {
		outmod = 0666;
		flag = ((xsymbol - usymtab) << 3) | 04;
	} else {
		flag &= ~TGLOBAL;
		if(flag >= TFLDST) {
			if(flag == TETEXT || flag == TEDATA) error('r');
			flag = 1;
		}
		if(flag >= TTEXT && flag <= TBSS) {
			if(!temp) val += DOTDOT;
		} else {
			if(temp) val -= DOTDOT;
		}
		if(--flag < 0) flag = 0;
	}
	flag <<= 1;
	flag |= temp;
	putw(val, &txtp);
	(*tseekp) += 2;
	putw(flag, &relp);
	(*rseekp) += 2;
}

outb(flag, val) {

	if(DOTREL != TBSS && flag > TABS) error('r');
	DOT++;
	if(passno == 0) return;
	if(!(DOT & 1)) {
		*((char *)txtp.f_ptr-1) = val;
		return;
	}
	putw(val, &txtp);
	putw(0, &relp);
	(*tseekp) += 2;
	(*rseekp) += 2;
}

error(chr) {

	errflg++;
	outmod = 0666;
	fprintf(stderr, "%s%c %d\n", argb, chr, line);
	argb[0] = 0;
}
