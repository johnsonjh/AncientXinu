#
/* PDP-11 Assembler for the VAX Pass 0 */

#include "vars1.h"

int eval;
char etype;

expres(nextop) {
	int op = 0;
	int opfound = 0;
	short leftval = 0, rightval;
	char lefttype = 1, righttype;

	for(;;) {
		if(op == 0) op = '+';
		else nextop = readop();
		if(nextop < 0 || nextop >= 0200) {
			righttype = ((struct sinfo *)nextop)->s_type;
			rightval = ((struct sinfo *)nextop)->s_value;
		} else if(nextop >= 0141) {
			if(nextop < 0141+10.) {
				nextop -= 0141;
				righttype = curfbr[nextop];
				if((rightval = curfb[nextop]) == -1)
					error('f');
			} else {
				lefttype = 0;
				leftval = 0;
			}
		} else switch(nextop) {
		    	case '+': case '-': case '*': case '/':
		    	case '&': case 037: case 035: case 036:
		    	case '%': case '^': case '!':
				if(op != '+') error('e');
				op = nextop;
				continue;

		    	case 1:
				rightval = numval;
				righttype = 1;
				break;

		    	case '[':
				if( (nextop = expres(readop())) != ']') error(']');
				rightval = eval;
				righttype = etype;
				break;

		    	default:
				if(!opfound) error('e');
				eval = leftval;
				etype = lefttype;
				return(nextop);
			}

		opfound++;
		switch(op) {

		    case '^':
			lefttype = righttype;
			break;

		    case 036:
			rightval = -rightval;
		    case 035:
			lefttype = combin(lefttype, righttype, 0);
			leftval = (unsigned)leftval << rightval;
			break;

		    case '%':
			lefttype = combin(lefttype, righttype, 0);
			leftval = (unsigned)leftval % rightval;
			break;

		    case '+':
			lefttype = combin(lefttype, righttype, 0);
			leftval += rightval;
			break;

		    case '-':
			lefttype = combin(lefttype, righttype, 1);
			leftval -= rightval;
			break;

		    case '&':
			lefttype = combin(lefttype, righttype, 0);
			leftval &= rightval;
			break;

		    case 037:
			lefttype = combin(lefttype, righttype, 0);
			leftval ^= rightval;
			break;

		    case '*':
			lefttype = combin(lefttype, righttype, 0);
			leftval *= rightval;
			break;

		    case '/':
			lefttype = combin(lefttype, righttype, 0);
			leftval = (unsigned)leftval / rightval;
			break;

		    case '!':
			lefttype = combin(lefttype, righttype, 0);
			leftval = ~leftval;
			break;
		}
	op = '+';
	}
}

combin(lefttype, righttype, flag) {
	int outtype, temp;

	outtype = (lefttype | righttype) & 040;
	lefttype &= 037;
	righttype &= 037;
	if(righttype > lefttype) {
		temp = righttype;
		righttype = lefttype;
		lefttype = temp;
	}
	if(righttype != 0) {
		if(flag && righttype == lefttype) lefttype = 1;
		return(lefttype | outtype);
	} else return(outtype);
}
