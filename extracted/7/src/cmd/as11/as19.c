#
/* PDP-11 Assembler for the VAX Pass 0 */

#include "vars1.h"
#include <signal.h>

#define PASS1		// Cause names to be inserted
#include "symbols.code"	// Insert symbol table

#define SYMSIZE (sizeof(symtab)/sizeof(struct symbol))

main(argc, argv) char **argv; {
	int (*oldsig)();
	int aexit();
	int chr;
	extern int end;

	oldsig = signal(SIGINT, SIG_IGN);
	if( ((int)oldsig & 01) == 0) signal(SIGINT, aexit);
	argv++;
	argc--;
	while(argv[0][0] == '-' && argc > 0) {
		chr = argv[0][1];
		if(chr == 'u' || chr == '\0') globfl[1] = 'g';
		else if(chr == 'o') {
			argv2[3] = *++argv;
			argc--;
		} else {
			fprintf(stderr, "as: %s bad flag\n", *argv);
			break;
		}
		argv++;
		argc--;
	}
	nargs = argc;
	curarg = argv - 1;
	symend = endcore = (char *)&end;
	usymtab = (struct symbol *)endcore;
	pof = fcreat(a_tmp1);
	setbuf(pof, _sobuf);
	fbfil = fcreat(a_tmp2);
	setbuf(fbfil, iobuf);
	setup();
	fclose(stdin);
	go();
}

setup() {
	struct symbol *sptr, **hptr, **findsym();

	for(sptr = symtab; sptr < &symtab[SYMSIZE]; sptr++) {
		hptr = findsym(hashsym(sptr->s_name), sptr->s_name);
		*hptr = sptr;
	}
}
