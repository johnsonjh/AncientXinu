#
/* PDP-11 Assembler for the VAX Pass 1 */

#include "vars2.h"
#include <stdio.h>

long lseek();

oset(file, pos) register struct afile *file; long pos; {
	file->f_seek = pos;
	file->f_end = (short *)(file + 1);
	file->f_ptr = (short *)((char *)file->f_buffer + (file->f_seek & 0777));
}

putw(word, file) register struct afile *file; {
	if(file->f_ptr >= file->f_end) flush(file);
	*file->f_ptr++ = word;
}

flush(file) register struct afile *file; {
	char *addr;

	if(lseek(fout, file->f_seek, 0) < 0) wrterr("can't seek %s", a_outp);
	addr = (char *)file->f_buffer + (file->f_seek & 0777);
	if(write(fout, addr, (char *)file->f_ptr - addr) < 0)
		wrterr("can't write on %s", a_outp);
	file->f_seek = (file->f_seek | 0777) + 1;
	file->f_ptr = file->f_buffer;
}

/* VARARGS1 */
wrterr(s, n)
char *s, *n;
{
	fprintf(stderr, "as: error on output: ");
	fprintf(stderr, s, n);
	fprintf(stderr, "\n");
	saexit();
}

readop() {
	int tmp;

	if((tmp = savop)) {
		savop = 0;
		return(tmp);
	}
	tmp = getshort();
	if(tmp == -1) return(ASEOF);
	if((unsigned)tmp < 0200) return(tmp);
	if((unsigned)tmp < 04000) return(tmp + (int)symtab - 01000);
	return(tmp + (int)usymtab - 04000);
}

getshort() {
	int tmp;

	if((tmp = savop)) {
		savop = 0;
		return(tmp);
	}
	if((tmp = getchar()) == -1) return(-1);
	return(tmp | (getchar() << 8) );
}
