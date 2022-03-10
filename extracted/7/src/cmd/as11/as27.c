#
/* PDP-11 Assembler for the VAX Pass 1 */

#include "vars2.h"

#define X -2
#define M -1

char reltp2[36] = {
	0, 0, 0, 0, 0, 0,
	0, M, 2, 3, 4, 040,
	0, 2, X, X, X, X, 
	0, 3, X, X, X, X,
	0, 4, X, X, X, X,
	0, 040, X, X, X, X, 
	};

char reltm2[36] = {
	0, 0, 0, 0, 0, 0,
	0, M, 2, 3, 4, 040,
	0, X, 1, X, X, X, 
	0, X, X, 1, X, X,
	0, X, X, X, 1, X,
	0, X, X, X, X, X, 
	};

char relte2[36] = {
	0, 0, 0, 0, 0, 0,
	0, M, X, X, X, X,
	0, X, X, X, X, X, 
	0, X, X, X, X, X,
	0, X, X, X, X, X,
	0, X, X, X, X, X, 
	};

struct symbol *xsymbol;

expres(nextop) {
	xsymbol = 0;
	return(expres1(nextop));
}

expres1(nextop) {
	int op = 0;
	short leftval = 0, rightval;
	char lefttype = 1, righttype;
	struct symbol *sptr;

	for(;;) {
		if(op == 0) op = '+';
		else nextop = readop();
		if((unsigned)nextop >= 0200) {
			sptr = (struct symbol *)nextop;
			righttype = sptr->s_type;
			if(righttype == 0 && passno) error('u');
			if(righttype == 040) {
				xsymbol = sptr;
				rightval = 0;
			} else rightval = sptr->s_value;
		} else if(nextop >= 0141) {
				sptr = curfb[nextop-0141];
				rightval = sptr->s_value;
				righttype = sptr->s_type;
		} else switch(nextop) {
			case '+': case '-': case '*': case '/':
			case '&': case 037: case 035: case 036:
			case '%': case '^': case '!':
				if(op != '+') error('e');
				op = nextop;
				continue;

			case 1:
				rightval = getshort();
				righttype = 1;
				break;

			case 2:
				rightval = numval;
				righttype = 1;
				break;

			case '[':
				if( (nextop = expres1(readop())) != ']')
					error(']');
				righttype = etype;
				rightval = eval;
				break;

			default:
				eval = leftval;
				etype = lefttype;
				return(nextop);
		}
		switch(op) {
		    case '+':
			lefttype = combin(lefttype, righttype, reltp2);
			leftval += rightval;
			break;

		    case '-':
			lefttype = combin(lefttype, righttype, reltm2);
			leftval -= rightval;
			break;

		    case '*':
			lefttype = combin(lefttype, righttype, relte2);
			leftval *= rightval;
			break;

		    case '/':
			lefttype = combin(lefttype, righttype, reltm2);
			leftval = (unsigned)leftval / rightval;
			break;

		    case 037:
			lefttype = combin(lefttype, righttype, relte2);
			leftval |= rightval;
			break;

		    case '&':
			lefttype = combin(lefttype, righttype, relte2);
			leftval &= rightval;
			break;

		    case 036:
			rightval = -rightval;
		    case 035:
			lefttype = combin(lefttype, righttype, relte2);
			leftval = (unsigned)leftval << rightval;
			break;

		    case '%':
			lefttype = combin(lefttype, righttype, relte2);
			leftval = (unsigned)leftval % rightval;
			break;

		    case '^':
			lefttype = righttype;
			break;

		    case '!':
			lefttype = combin(lefttype, righttype, relte2);
			leftval += (rightval = ~rightval);
			break;
		}
	op = '+';
	}
}

int maxtype;

combin(lefttype, righttype, table) char *table; {
	int outtype, temp;

	if(passno == 0) {
		outtype = (lefttype | righttype) & 040;
		lefttype &= 037;
		righttype &= 037;
		if(righttype > lefttype) {
			temp = righttype;
			righttype = lefttype;
			lefttype = temp;
		}
		if(righttype != 0) {
			if(table == reltm2 && righttype == lefttype)
				lefttype = 1;
			return(lefttype | outtype);
		}
		return(outtype); 
	} else {
		maxtype = 0;
		righttype = maprel(righttype) * 6;
		lefttype = table[maprel(lefttype) + righttype];
		if(lefttype < 0) {
			if(lefttype != M) error('r');
			lefttype = maxtype;
		}
		return(lefttype);
	}
}

maprel(type) {
	if(type == 040) return(5);
	type &= 037;
	if(type > maxtype) maxtype = type;
	if(type >= 5) type = 1;
	return(type);
}
