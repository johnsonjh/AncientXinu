/* PDP-11 Assembler for the VAX Pass 0 */

#include "vars1.h"
#include "stypes.h"

struct sinfo nxtfb;
char curfbr[10];
short curfb[10] = {
	-1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1,
	};

assem() {
	int op, svop;

  astart:
	for(;;) {
		op = readop();
		if(checkeos(op)) goto ealoop;
		if(ifflg == 0) break;
		if(op <= 0200) continue;
		if( ((struct sinfo *)op)->s_type == TCIF) {
			ifflg++;
		}
		else if( ((struct sinfo *)op)->s_type == TCENDIF) {
			ifflg--;
		}
	}
	svop = op;
	op = readop();
	if(op == '=') {
		op = expres(readop());
		if(svop < 0200) {
			error('x');
			goto ealoop;
		}
		if( &((struct sinfo *)svop)->s_type == &DOTREL) {
			etype &= ~ 040;
			if(etype != DOTREL) {
				error('.');
				DOTREL = 02;
				goto ealoop;
			}
		}
		((struct sinfo *)svop)->s_type &= ~037;
		if( (etype &= 037) == 0) eval = 0;
		((struct sinfo *)svop)->s_type |= etype;
		((struct sinfo *)svop)->s_value = eval;
		goto ealoop;
	}		
	if(op == ':') {
		if( (op = svop) >= 0200) {
			if( ((struct sinfo *)op)->s_type & 037) error('m');
			((struct sinfo *)op)->s_type |= DOTREL;
			((struct sinfo *)op)->s_value = DOT;
			goto astart;
		}
		if(op != 1) {
			error('x');
			goto astart;
		}
		if(numval > 9) {
			error('f');
			numval = 0;
		}
		curfbr[numval] = DOTREL;
		nxtfb.s_type = DOTREL;
		nxtfb.s_value = DOT;
		nxtfb.s_index = numval * 2;
		curfb[numval] = DOT;
		if(fwrite((char *)&nxtfb, sizeof(nxtfb), 1, fbfil) != 1)
			wrterr("can't write on %s", a_tmp2);
		goto astart;
	}
	saveop = op;
	op = opline(svop);
  ealoop:
	if(op == ';') goto astart;
	if(op == '\n') {
		line++;
		goto astart;
	}
	if(op != ASEOF) {
		error('x');
		while(!checkeos(readop())) ;
	}
	if(ifflg) error('x');
}

checkeos(op) {
	return( op == '\n' || op == ';' || op == ASEOF);
}
