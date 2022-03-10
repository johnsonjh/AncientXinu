#
/* PDP-11 Assembler for the VAX Pass 0 */

#include "vars1.h"

error(chr) {

	errflg++;
	if(*curarg) {
		fprintf(stderr, "%s\n", *curarg);
		*curarg = NULL;
	}
	fprintf(stderr, "%c %d\n", (int)chr, line);
}

putshort(word) short word; {
	register int temp;

	if(ifflg && word != '\n') return;
	temp = word & 0377;
	putc(temp, pof);
	temp = (word >> 8) & 0377;
	putc(temp, pof);
	if(ferror(pof))
		wrterr("error writing %s", a_tmp1);
}
