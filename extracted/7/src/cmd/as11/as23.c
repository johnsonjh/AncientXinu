#
/* PDP-11 Assembler for the VAX Pass 1 */

#include "vars2.h"

assem() {
	int op, svop, temp;
	struct symbol *sptr;

  astart:
	op = readop();
	if(op != 05 && op != '<') {
		if(checkeos(op)) goto ealoop;
		if( (svop = op) == 1) {
			svop = 2;
			numval = getshort();
		}
		op = readop();
		if(op == '=') {
			op = expres(readop());
			sptr = (struct symbol *)svop;
			if(&sptr->s_value == &DOT) {
				etype &= ~040;
				if(etype != DOTREL) {
					error('.');
					goto ealoop;
				}
				if(etype == 04) {
					DOT = eval;
					goto dotmax;
				}
				eval -= DOT;
				if(eval < 0) {
					error('.');
					goto ealoop;
				}
				while(eval-- > 0) outb(1, 0);
				goto dotmax;
			}
			if(etype == 040) error('r');
			sptr->s_type &= ~037;
			if((etype &= 037) == 0) eval = 0;
			sptr->s_type |= etype;
			sptr->s_value = eval;
			goto ealoop;
		} else if(op == ':') {
			if(svop < 0200) { 
				if(svop != 02) error('x');
				else {
					fbadv(numval);
					sptr = curfb[numval];
					sptr->s_type = DOTREL;
					brdelt = sptr->s_value - DOT;
					sptr->s_value = DOT;
				}
				goto astart;
			}
			sptr = (struct symbol *)svop;
			if(passno) {
				if(DOT != sptr->s_value)
					error('p');
				goto astart;
			}
			temp = sptr->s_type & 037;
			if(temp != 0 && temp != 033 && temp != 034)
				error('m');
			sptr->s_type &= ~037;
			sptr->s_type |= DOTREL;
			brdelt = sptr->s_value - DOT;
			sptr->s_value = DOT;
			goto astart;
		} else {
			savop = op;
			op = svop;
		}
	}
	op = opline(op);
 dotmax:
	if(passno == 0 && ((short *)&TXTSIZ)[DOTREL-2] <= DOT)
		((short *)&TXTSIZ)[DOTREL-2] = DOT;
 ealoop:
	if(op == '\n') {
		line++;
		goto astart;
	}
	if(op != ASEOF) goto astart;
}

checkeos(op) {
	return(op == '\n' || op == ';' || op == ASEOF);
}

fbadv(num) {
	struct symbol *sptr;

	sptr = nxtfb[num];
	if((curfb[num] = sptr) == 0) sptr = fbbufp;
	else sptr++;
	while(sptr->s_index != (num << 1) && sptr->s_index >= 0) sptr++;
	nxtfb[num] = sptr;
}
