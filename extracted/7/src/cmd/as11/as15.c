#
/* PDP-11 Assembler for the VAX Pass 0 */

#include "vars1.h"

char chartab[128] = {
	CTRL, CTRL, CTRL, CTRL, TERM, CTRL, CTRL, CTRL,
	CTRL, BLNK, TERM, CTRL, CTRL, BLNK, CTRL, CTRL,
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL,
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL,
	BLNK, OPER, QUOT, CTRL, OPER, OPER, OPER, SQOT,
	OPER, OPER, OPER, OPER, OPER, OPER, IDEN, COMM,
	IDEN, IDEN, IDEN, IDEN, IDEN, IDEN, IDEN, IDEN,
	IDEN, IDEN, OPER, TERM, STRN, OPER, CTRL, CTRL,
	CTRL, IDEN, IDEN, IDEN, IDEN, IDEN, IDEN, IDEN,
	IDEN, IDEN, IDEN, IDEN, IDEN, IDEN, IDEN, IDEN,
	IDEN, IDEN, IDEN, IDEN, IDEN, IDEN, IDEN, IDEN,
	IDEN, IDEN, IDEN, OPER, ESCP, OPER, OPER, IDEN,
	CTRL, IDEN, IDEN, IDEN, IDEN, IDEN, IDEN, IDEN,
	IDEN, IDEN, IDEN, IDEN, IDEN, IDEN, IDEN, IDEN,
	IDEN, IDEN, IDEN, IDEN, IDEN, IDEN, IDEN, IDEN,
	IDEN, IDEN, IDEN, CTRL, ORSY, CTRL, IDEN, CTRL,
	};

static char strterm;
int saveop;

readop() {
	int val;

	if(val = saveop) {
		saveop = 0;
		return(val);
	}
   again:
	val = rch();
	switch(chartab[val]) {
	    case CTRL:
		error('g');

	    case BLNK:
		goto again;

	    case COMM:
		while((val = rch()) != ASEOF && val != '\n') ;

	    case TERM:
	    case OPER:
		break;

	    case QUOT:
		val = rsch();
		return(snumval(val | (rsch() << 8)));

	    case SQOT:
		return(snumval(rsch()));

	    case IDEN:
		ch = val;
		if(ch >= '0' && ch <= '9') {
			val = number();
			if(numsymbol) break;
			return(snumval(val));
		}
		return(rname());

	    case STRN:
		putshort('<');
		numval = 0;
		for(;;) {
			val = rsch();
			if(strterm) {
				putshort(-1);
				return('<');
			}
			putshort(0400 | val);
			numval++;
		}

	    case ESCP:
		val = escp();
		break;

	    case ORSY:
		val = 037;
		break;
	}
	putshort(val);
	return(val);
}

snumval(word) {
	numval = word;
	putshort(1);
	putshort(numval);
	return(1);
}

char schar[] = {
	'n', 012,
	's', 040,
	't', 011,
	'e', 004,
	'0', 000,
	'r', 015,
	'a', 006,
	'p', 033,
	0 };

rsch() {
	int chr;
	char *cptr;

	strterm = 0;
	chr = rch();
	if(chr == ASEOF || chr == '\n') {
		error('<');
		aexit();
	}
	if(chr != '\\') { 
		if(chr == '>') strterm++;
		return(chr);
	}
	chr = rch();
	for(cptr = schar; *cptr != '\0'; cptr++)
		if(*cptr++ == chr) return(*cptr);
	return(chr);
}

char esctab[] = {
	'/', '/',
	'<', 035,
	'>', 036,
	'%', 037,
	0 };

escp() {
	int chr;
	char *cptr;

	chr = rch();
	for(cptr = esctab; *cptr != '\0'; cptr++)
		if(*cptr++ == chr) return(*cptr);
	return(chr);
}
