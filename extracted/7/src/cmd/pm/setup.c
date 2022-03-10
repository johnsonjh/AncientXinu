#include "pm.h"
#include <sys/stat.h>

/*
 * pm - initialization routines
 */

struct	nlist	nl[] =		/* symbols pulled out of the text image	 */
{
	{"_currpid"},
#define	CURRPID		0
	{"_proctab"},
#define	PROC		1
	{"_q"},
#define	XQ		2
	{"_semaph"},
#define	SEMS		3
	{"_tty"},
#define	TTYS		4
	{"_vers"},
#define	VERS		5
	{"_clktime"},
#define	CLKTIM		6
	0
};
extern	struct	pentry	*proctb;
extern	struct	sentry	*semtab;
extern	struct	tty	*tptr;
extern	struct	qent	*Q;
extern	char	*vers;
extern	long	clktim;

setup(argc, argv)
int	argc;
char	*argv[];
{
	char		*malloc();
	FILE		*txtfd, *corefd;
	struct stat	stat;
	register struct nlist	*sp1, *sp2;
	register 	i;
	int		s1, s2;
	int		svalcomp();
	
	procargs(argc, argv);

	if (verbose)
		printf("Opening core file...\n");
	corefd = fopen(corefile, "r");
	if(corefd == (FILE *)NULL){
		fprintf(stderr, "Can't open %s\n", corefile);
		exit(-1);
	}
	
	if (verbose)
		printf("Opening text file...\n");
	txtfd = fopen(txtfile, "r");
	if(txtfd == (FILE *)NULL){
		fprintf(stderr, "Can't open %s\n", txtfile);
		exit(-2);
	}

	if (verbose)
		printf("Reading core header of %d bytes\n",
			sizeof(struct core11) );
	fread(&c_header, sizeof(struct core11), 1, corefd);/* read the header */
	
	fstat(fileno(corefd), &stat);
	if ( stat.st_size <= sizeof(struct core11) ) {
		fprintf(stderr,"Malformed core file header (size=%d)\n",
			stat.st_size);
		exit(-2);
	}
	stat.st_size -= sizeof(struct core11);
	if (verbose)
		printf("Allocating %d bytes for core image\n",stat.st_size);
	core = (short *)malloc(stat.st_size);
	if(core == (short *)NULL){
		fprintf(stderr, "Can't allocate memory for core image\n");
		exit(-3);
	}

	if (verbose)
		printf("Reading core image...\n");
	fread(core, sizeof(char), stat.st_size, corefd);/* read the rest */

	if (verbose)
		printf("Reading a.out header...\n");
	fread(&a_out, sizeof(struct exec), 1, txtfd);		
	text = (short *)malloc(a_out.a_text + a_out.a_data + a_out.a_bss);
	if(text == (short *)NULL){
		fprintf(stderr, "Can't allocate space for system image\n");
		exit(-4);
	}
	fread(text, sizeof(char), a_out.a_text + a_out.a_data, txtfd);
	
	symtab = (struct nlist *)malloc(a_out.a_syms);
	esymtab = &symtab[a_out.a_syms / sizeof (struct nlist)];
	if(a_out.a_syms == 0)
	       	printf("No symbol table\n");
	if(symtab == (struct nlist *)NULL){
		fprintf(stderr, "Can't allocate space for symbol table\n");
		exit(-5);
	}
	
	fread(symtab, a_out.a_syms, sizeof(char), txtfd);
	
	/*
	 * produce list of externals sorted by address 
	 */

	ssymtab = (struct nlist *)malloc(a_out.a_syms);
	i = 0;
	for(sp1 = symtab, sp2 = ssymtab; sp1 < esymtab; sp1++){
		if((sp1->n_type & N_EXT) == 0)
			continue;
		i++;
		*sp2++ = *sp1;
	}

	essymtab = ssymtab + i;

	qsort(ssymtab, i, sizeof(struct nlist), svalcomp);

	/* 
	 * look up addresses of interesting tables 
	 */

	nlist11(txtfile, nl);
	currpid = core[nl[CURRPID].n_value >> 1];
	proctb = (struct pentry *)(&core[nl[PROC].n_value >> 1]);
	semtab = (struct sentry *)(&core[nl[SEMS].n_value >> 1]);
	Q = (struct qent *)(&core[nl[XQ].n_value >> 1]);
        tptr = (struct tty *)(&core[nl[TTYS].n_value >> 1]);
	vers = nl[VERS].n_value == 0 ? "6" :
		(char *)(&core[nl[VERS].n_value >> 1]);
	if (nl[CLKTIM].n_value == 0) {
		clktim = 0L;
	} else {
		s1 = 0xffff & (*((short *) &core[  (nl[CLKTIM].n_value>>1)]));
		s2 = 0xffff & (*((short *) &core[1+(nl[CLKTIM].n_value>>1)]));
		clktim = (s1<<16) | s2;
	}
}

svalcomp(sp1, sp2)
register struct nlist *sp1, *sp2;
{
	if(sp1->n_value == sp2->n_value)
		return(0);
	if(sp1->n_value < sp2->n_value)
		return(-1);
	return(1);
}

int a_magic[] = {A_MAGIC1, A_MAGIC2, A_MAGIC3, A_MAGIC4, 0};
#define SPACE 100		/* number of symbols read at a time */

nlist11(name, list)
char *name;
struct nlist *list;
{
	register struct nlist *p, *q;
	int f, n, m, i;
	long sa;
	struct exec buf;
	struct nlist space[SPACE];

	for(p = list; p->n_name[0]; p++) {
		p->n_type = 0;
		p->n_value = 0;
	}
	f = open(name, 0);
	if(f < 0)
		return(-1);
	read(f, (char *)&buf, sizeof buf);
	for(i=0; a_magic[i]; i++)
		if(a_magic[i] == buf.a_magic) break;
	if(a_magic[i] == 0){
		close(f);
		return(-1);
	}
	sa = buf.a_text + (long)buf.a_data;
	if(buf.a_flag != 1) sa *= 2;
	sa += sizeof buf;
	lseek(f, sa, 0);
	n = buf.a_syms;

	while(n){
		m = sizeof space;
		if(n < sizeof space)
			m = n;
		read(f, (char *)space, m);
		n -= m;
		for(q = space; (m -= sizeof(struct nlist)) >= 0; q++) {
			for(p = list; p->n_name[0]; p++) {
				for(i=0;i<8;i++)
					if(p->n_name[i] != q->n_name[i]) goto cont;
				p->n_value = q->n_value;
				p->n_type = q->n_type;
				break;
		cont:		;
			}
		}
	}
	close(f);
	return(0);
}

/*
 *===================================================
 * procargs - unified argument processing procedure
 *===================================================
 *
 * This procedure continas the logic for converting the UNIX argument
 * list into global variables
 */

procargs(argc, argv)
int argc;
char *argv[];
{
        int arg, unswitched, more;
        char *swptr;

	corefile = "core11";
	txtfile = "a.out";
	allopts = 1;
	popt = sopt = topt = 0;
	verbose = 0;

        unswitched = 0;
        for ( arg=1 ; arg<argc ; arg++ ) {
                if ( argv[arg][0] == '-' ) {
                        more = 1;
                        swptr = &argv[arg][1];
                        while ( more && *swptr!='\0' ) {
                                switch ( *swptr++ ) {
				case 'v':
					verbose=1;

				case 'p':
					allopts = 0;
					popt = 1;
					break;

				case 's':
					allopts = 0;
					sopt = 1;
					break;

				case 't':
					allopts = 0;
					topt = 1;
					break;

                                default:
                                        usagexit(argv[0]);
                                }
                        }
                } else { /* there's no dash in front */
                        switch ( unswitched++ ) {
			case 0:
				txtfile = argv[arg];
				break;
			
			case 1:
				corefile = argv[arg];
				break;

                        default:
                                usagexit(argv[0]);
                        }
                }
        }
}
usagexit(pgm)
char *pgm;
{
        fprintf(stderr,"usage: %s [program [core]]\n",pgm);
        exit(1);
}
