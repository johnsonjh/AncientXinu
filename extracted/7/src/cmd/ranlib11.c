/* This is RANLIB11 -- Which works on PDP 11 libraries stored
 * on a VAX in the portable ARchive format.
 *  
 * Adapted from software received from Eric Cooper of 
 * the University of California at Berkeley.
 *  
 * Steve Salisbury      October 1981
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <ar.h>
#include "/usr/Xinu/include/a.out.h"

#define MAGIC   exp.a_magic
#define BADMAG	(MAGIC!=A_MAGIC1 && MAGIC!=A_MAGIC2 && MAGIC!=A_MAGIC3 && MAGIC!=A_MAGIC4)
#define ARXFILNAME      "__.SYMDEF     "
#define XFILNAME        "__.SYMDEF"

struct	ar_hdr	arp;
long	arp_size;	/* value of "ar_size" field of current arp */
struct	exec	exp;
long	off, oldoff;
long	ftell();
int	tabsize = 1000;
struct tab {
	char t_name[8];
	long t_loc;
} *tab;
int	tnum;
int	new;
char	firstname[15];
int	cmpel();

main(argc, argv)
char **argv;
{
	char buf[256];
        char magic[SARMAG];

	while(--argc) {
		if (isdigit(**++argv)) {	/* table size */
			tabsize = atoi(*argv);
			continue;
		}
		if (freopen(*argv,"r", stdin) == NULL) {
			fprintf(stderr, "cannot open %s\n", *argv);
			continue;
		}
		off = SARMAG;
                fread(magic, 1, SARMAG, stdin);   /* magic no. */
                if (strcmpn(magic,ARMAG,SARMAG)) {
			fprintf(stderr, "not archive: %s\n", *argv);
			continue;
		}
		tab = (struct tab *)calloc(tabsize, sizeof(struct tab));
                if (tab == NULL) {
                        printf("Out of space\n");
                        exit(1);
                        }
		new = tnum = 0;
		fseek(stdin, 0L, 0);
		if (nextel() == 0)
			continue;
		do {
			register n;
                        long     o;

			fread((char *)&exp, 1, sizeof(struct exec), stdin);
			if (BADMAG)		/* archive element not in  */
				continue;	/* proper format - skip it */
                        o = (long) exp.a_text + exp.a_data;
                        if( (exp.a_flag & 01) == 0) 
                                o *= 2;
                        fseek(stdin, o, 1);
                        n = exp.a_syms / sizeof(struct nlist);
			if (n == 0) {
				fprintf(stderr, "%s: no name list\n", arp.ar_name);
				continue;
			}
			while (--n >= 0)
				stash();
		} while (nextel());
		if (wrtable() == 0) {
			if (new)
                                sprintf(buf, "ar rb %s %s %s\n", firstname, *argv, XFILNAME);
                        else                        
                                sprintf(buf, "ar r %s %s\n", *argv, XFILNAME);
			if (system(buf))
				fprintf(stderr, "can't execute %s\n", buf);
			else {
				fixdate(*argv);
				unlink(XFILNAME);
			}
		}
	}
	return (0);
}

nextel()
{
	register r;

	oldoff = off;
	fseek(stdin, off, 0);
	r = fread((char *)&arp, 1, sizeof(struct ar_hdr), stdin);
	arp_size = atoi(arp.ar_size);
	if (r <= 0)
		return (0);
	if (arp_size & 1)
		++arp_size;
	off = ftell(stdin) + arp_size;	/* offset to next element */
	return (1);
}

stash() 
{
	register struct tab *tp;
	struct nlist sym;
	char t;

        if (tnum >= tabsize) {
                printf("symbol table overflow\n");
                exit(1);
                }
	tp = &tab[tnum];
	fread((char *)&sym, 1, sizeof(struct nlist), stdin);
	t = sym.n_type;
	if ((t&N_EXT) && t != N_EXT+N_UNDF) {
		strcpyn(tp->t_name, sym.n_name, 8);
		tp->t_loc = oldoff;
		tnum++;
	}
}

/* patch time */
fixdate(s) 
char *s;
{
	long timex, time();
	int fd;
	struct ar_hdr timehdr; /* use only the time field */

	fd = open(s, 1);
	if(fd < 0) {
		fprintf(stderr, "can't reopen %s\n", s);
		return;
	}
	timex = time(NULL) + 5; /* should be enough time */
	sprintf(timehdr.ar_date, "%-12ld", timex);
	lseek(fd, (long)SARMAG + (arp.ar_date-(char *)&arp), 0);
	write(fd, timehdr.ar_date, sizeof(timehdr.ar_date));
	close(fd);
}

int	multiple;

wrtable()
{
	FILE *fo;
	register struct tab *tp;
	long offdelta;

	multiple = 0;
	qsort((char *)tab, tnum, sizeof(struct tab), cmpel);
	if (multiple)
		return (1);

/* fix up offsets now that we know the total size */
	offdelta = tnum * sizeof(struct tab) + sizeof(arp);
	off = SARMAG;
	nextel();
        if (strcmpn(arp.ar_name, ARXFILNAME, 14) == 0) {
		new = 0;
		offdelta -= sizeof(arp) + arp_size;
	}
	else {
		new = 1;
		strcpyn(firstname, arp.ar_name, 14);
	}
	for (tp = tab; tp < &tab[tnum]; tp++)
		tp->t_loc += offdelta;
				
/* write out the entries */
	fo = fopen(XFILNAME, "w");
        if(fo == NULL) {
                printf("can't create %s\n", XFILNAME);
                exit(1);
                }
	fwrite(tab, tnum, sizeof(struct tab), fo);
	fclose(fo);
}

cmpel(p, q)
register struct tab *p, *q;
{
	register long n;

	if ((n = strcmpn(p->t_name, q->t_name, 8)) == 0)
		if ((n = p->t_loc - q->t_loc) != 0) {
			fprintf(stderr, "%.8s multiply defined\n",
				p->t_name);
			multiple = 1;
		}
	if (n > 0)
		return (1);
	if (n < 0)
		return (-1);
	return (0);
}

