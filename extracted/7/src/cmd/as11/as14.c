#
/* PDP-11 Assembler for the VAX Pass 0 */

#include "vars1.h"

hashsym(ptr) register char *ptr; {
	register int i, hash = 0;

	for(i = 8; (i > 0) && (*ptr != '\0'); i--) {
		hash += *ptr++;
		hash = ((hash & 0377) << 8) | ((hash >> 8) & 0377);
	}
	return(hash);
}

struct symbol **
findsym(hash, name) char *name; {
	int hashinc, times = 0;
	struct symbol **hptr;

	hashinc = hash/HSHSIZE;
	hash %= HSHSIZE;
	for(hptr = &hshtab[hash];;hptr -= hashinc) {
		if(hptr <= hshtab) {
			if(times) {
				fprintf(stderr, "as: symbol table overflow\n");
				aexit();
			}
			hptr += HSHSIZE;
			times++;
		}
		if((*--hptr == 0) || namcmp(name, (*hptr)->s_name))
			return(hptr);
	}
}

rname() {
	int local = 0, hash;
	register char *symptr;
	struct symbol **hptr;
	register int chr;

	for(symptr = (char *)&symbol; symptr < (char *)(&symbol + 1); )
		*symptr++ = 0;
	symptr = symbol.s_name;
	if(ch == '~') {
		ch = 0;
		local++;
	}
	for( ; (chartab[ (chr = rch()) ] == IDEN); ) {
		if(symptr == &symbol.s_name[8]) continue;
		*symptr++ = chr;
	}
	ch = chr;
	hash = hashsym(symbol.s_name);
	if(!local) {
		hptr = findsym(hash, symbol.s_name);
		if(*hptr != 0) {
			symptr = (char *)*hptr;
			goto found;
		}
		*hptr = (struct symbol *)symend;
	}
	if(symend + sizeof(struct symbol) > endcore)
		endcore = sbrk(512);
	for(symptr = (char *)&symbol; symptr < (char *)(&symbol + 1);)
		*symend++ = *symptr++;
	symptr = symend - sizeof(symbol);
  found:
	if(symptr >= (char *)usymtab)
		putshort(04000 + ((symptr - (char *)usymtab) / 3) );
	else 
		putshort(01000 + ((symptr - (char *)symtab) / 3) );
	return( (int)(&((struct symbol *)symptr)->s_info) );
}


namcmp(chr1, chr2) register char *chr1, *chr2; {
	register int i;

	for(i = 8; i > 0; i--)
		if(*chr1++ != *chr2++) return(0);
	return(1);
}

char numsymbol;

number() {
	int oval = 0, dval = 0;
	int chr;

	numsymbol = 0;
	while((chr = rch()) >= '0' && chr <= '9') {
		chr -= '0';
		oval = (oval << 3) + chr;
		dval = (dval * 10) + chr;
	}
	if(chr== 'b' || chr == 'f') {
		if(dval > 9) {
			error('f');
			dval = 0;
		}
		dval += 0141;
		if(chr == 'f') dval += 10;
		numsymbol = 1;
		return(dval);
	}
	if(chr == '.') return(dval);
	ch = chr;
	return(oval);
}

rch() {
	int tmp;
	char *cp;

	if(tmp = ch) {
		ch = 0;
		return(tmp);
	}
	do {
		if((tmp = getchar()) == EOF) {
			if(nargs-- <= 0) return(ASEOF);
			if(ifflg) {
				error('i');
				aexit();
			}
			curarg++;
			fileflg++;
			if(freopen(*curarg, "r", stdin) == NULL) {
				fprintf(stderr, "%s?\n", *curarg);
				aexit();
			}
			line = 1;
			putshort(5);
			for(cp = *curarg; *cp != '\0';)
				putshort(*cp++);
			putshort(-1);
			tmp = 0;
		}
	} while(tmp == 0);
	return(tmp);
}
