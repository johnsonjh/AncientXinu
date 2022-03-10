#include <stdio.h>
#include <a.out.h>

/* dump text portion of .o file as instructions and symbols ... */

long	dot;
FILE	*infile;
char	objcode[100];
char	text[100];

char *badop = "\t???";
#define IMDF "#0x%x"			/* immediate data format */

char *bname[16] = { 
	"ra", "sr", "hi", "ls", "cc", "cs", "ne",
	"eq", "vc", "vs", "pl", "mi", "ge", "lt", "gt", "le" };

char *ccname[16] = { 
	"t", "f", "hi", "ls", "cc", "cs", "ne",
	"eq", "vc", "vs", "pl", "mi", "ge", "lt", "gt", "le" };

char *shro[4] = { 
	"as", "ls", "rox", "ro" };

char *bit[4] = { 
	"btst", "bchg", "bclr", "bset" };

int omove(),obranch(),oimmed(),oprint(),oneop(),soneop(),oreg(),ochk();
int olink(),omovem(),oquick(),omoveq(),otrap(),oscc(),opmode(),shroi();
int extend(),biti(), odbcc(), omoveto(), omovefrom();

struct opdesc
{			
	unsigned short mask, match;
	int (*opfun)();
	char *farg;
} 
opdecode[] =
{					/* order is important below */
	0xFF00, 0x0000, oimmed, "or",		/* op class 0  */
	0xFF00, 0x0200, oimmed, "and",
	0xFF00, 0x0400, oimmed, "sub",
	0xFF00, 0x0600, oimmed, "add",
	0xFF00, 0x0A00, oimmed, "eor",
	0xFF00, 0x0C00, oimmed, "cmp",
	0xF100, 0x0100, biti, 0,
	0xF800, 0x0800, biti, 0,
	0xF000, 0x1000, omove, "b",		/* move instructions */
	0xF000, 0x2000, omove, "l",
	0xF000, 0x3000, omove, "w",
	0xF000, 0x6000, obranch, 0,		/* branches */
	0xFFC0, 0x40C0, omovefrom, "sr", /* op class 4 */
	0xFF00, 0x4000, soneop, "negx",
	0xFF00, 0x4200, soneop, "clr",
	0xFFC0, 0x44C0, omoveto, "ccr",
	0xFF00, 0x4400, soneop, "neg",
	0xFFC0, 0x46C0, omoveto, "sr",
	0xFF00, 0x4600, soneop, "not",
	0xFFC0, 0x4800, oneop, "nbcd",
	0xFFF8, 0x4840, oreg, "\tswap\td%D",
	0xFFC0, 0x4840, oneop, "pea",
	0xFFF8, 0x4880, oreg, "\textw\td%D",
	0xFFF8, 0x48C0, oreg, "\textl\td%D",
	0xFB80, 0x4880, omovem, 0,
	0xFFC0, 0x4AC0, oneop, "tas",
	0xFF00, 0x4A00, soneop, "tst",
	0xFFF0, 0x4E40, otrap, 0,
	0xFFF8, 0x4E50, olink, 0,
	0xFFF8, 0x4880, oreg, "\textw\td%D",
	0xFFF8, 0x48C0, oreg, "\textl\td%D",
	0xFFF8, 0x4E58, oreg, "\tunlk\ta%D",
	0xFFF8, 0x4E60, oreg, "\tmove\ta%D,usp",
	0xFFF8, 0x4E68, oreg, "\tmove\tusp,a%D",
	0xFFFF, 0x4E70, oprint, "reset",
	0xFFFF, 0x4E71, oprint, "nop",
	0xFFFF, 0x4E72, oprint, "stop",
	0xFFFF, 0x4E73, oprint, "rte",
	0xFFFF, 0x4E75, oprint, "rts",
	0xFFFF, 0x4E76, oprint, "trapv",
	0xFFFF, 0x4E77, oprint, "rtr",
	0xFFC0, 0x4E80, oneop, "jsr",
	0xFFC0, 0x4EC0, oneop, "jmp",
	0xF1C0, 0x4180, ochk, "chk",
	0xF1C0, 0x41C0, ochk, "lea",
	0xF0C8, 0x50C8, odbcc, 0,
	0xF0C0, 0x50C0, oscc, 0,
	0xF100, 0x5000, oquick, "addq",
	0xF100, 0x5100, oquick, "subq",
	0xF000, 0x7000, omoveq, 0,
	0xF1C0, 0x80C0, ochk, "divu",
	0xF1C0, 0x81C0, ochk, "divs",
	0xF1F0, 0x8100, extend, "sbcd",
	0xF000, 0x8000, opmode, "or",
	0xF1C0, 0x91C0, opmode, "sub",
	0xF130, 0x9100, extend, "subx",
	0xF000, 0x9000, opmode, "sub",
	0xF1C0, 0xB1C0, opmode, "cmp",
	0xF138, 0xB108, extend, "cmpm",
	0xF100, 0xB000, opmode, "cmp",
	0xF100, 0xB100, opmode, "eor",
	0xF1C0, 0xC0C0, ochk, "mulu",
	0xF1C0, 0xC1C0, ochk, "muls",
	0xF1F8, 0xC188, extend, "exg",
	0xF1F8, 0xC148, extend, "exg",
	0xF1F8, 0xC140, extend, "exg",
	0xF1F0, 0xC100, extend, "abcd",
	0xF000, 0xC000, opmode, "and",
	0xF1C0, 0xD1C0, opmode, "add",
	0xF130, 0xD100, extend, "addx",
	0xF000, 0xD000, opmode, "add",
	0xF100, 0xE000, shroi, "r",
	0xF100, 0xE100, shroi, "l",
	0, 0, 0, 0
};

struct	nlist 	*Syms, *Symsort;
struct	relocation_info	*Relocation;
int	Relsize;
int 	Symsize;
char 	*Strings;
int 	Strsize;
char 	*textbase;

main(argc,argv)
int	argc;
char	*argv[];
{
	struct 	exec 	a;
	struct 	nlist 	*s, *t;
	int 	nlistcomp();

	setbuf(stdout, NULL);
	if (argc == 2) {
		if ((infile = fopen(argv[1],"r")) == NULL) {
			fprintf(stderr,"can't open %s for input\n",argv[1]);
			exit(1);
		}
	} 
	else infile = stdin;

	fread(&a, sizeof(struct exec), 1, infile);

	if (N_BADMAG(a)) {
		fprintf(stderr,"file is not in a.out format\n");
		exit(1);
	}
	printf("%d Bytes in the text segment\n",a.a_text);
			
	/* Get relocation information */
	Relsize = (a.a_trsize + a.a_drsize) / sizeof (struct relocation_info);
	Relocation = (struct relocation_info *) malloc(a.a_trsize + a.a_drsize);
	fseek(infile, sizeof (struct exec) + a.a_text + a.a_data, 0);
	fread(Relocation, sizeof (struct relocation_info), Relsize, infile);

	/* Get the symbol table */
	Symsize = a.a_syms/sizeof(struct nlist);
	Syms = (struct nlist *)malloc(a.a_syms);
	Symsort = (struct nlist *)malloc(a.a_syms);
	fseek(infile, N_SYMOFF(a),0);
	fread(Syms, 1, a.a_syms, infile);

			/* Get the size of the string table */
	fseek(infile, N_STROFF(a), 0);
	fread(&Strsize, sizeof(int), 1, infile);
			/* Now, the string table itself */
	Strings = (char *)malloc(Strsize);
	fread(Strings, 1, Strsize, infile);
			/* Adjust the string offsets in the symbol table
			to actual char pointers into the strings table */
	for (s=Syms, t=Symsort; s < &Syms[Symsize]; s++, t++) {
		s->n_un.n_name = (char *)(s->n_un.n_strx + (int)Strings -
		    (sizeof(Strsize)));
		*t = *s;
	}
	qsort(Symsort, Symsize, sizeof (struct nlist), nlistcomp);
			/* Seek back to the text segment */
		
	fseek(infile, N_TXTOFF(a), 0);
	/* Sort the symbol table by value */
	dot = a.a_entry;
	while (dot < a.a_text+a.a_entry) {
		objcode[0] = '\0';
		text[0] = '\0';
		pexact(dot, 1);
		if( strlen(text) < 7)
			addtext(":\t");
		else
			addtext(":");
		printins(instfetch(2));
		strncat(text, "                                        ",
			40 - mystrlen(text));
		strcat(text, objcode);
		printf("%s\n", text);
	}
	exit(0);
}

nlistcomp(a,b)
struct nlist *a,*b;
{
	return(a->n_value - b->n_value);
}

/* print symbol if we're right on top of it... */
pexact(val, type)
register int	val;
register int	type;
{
	register struct	nlist	*s;
	register int	index;

	if (val != 0  ||  (index = getsymnum()) == -1) {
		for (s = Symsort; s < &Symsort[Symsize]; s++) {
			if (val == s->n_value) {
				if (type && (s->n_type != type))
					break;
				addtext("%s",s->n_un.n_name);
				return;
			}
			else if (val < s->n_value) break;
		}
		addtext("0x%x",val);
	} else {
		addtext("%s", Syms[index].n_un.n_name);
	}
}

getsymnum()
{
	struct	relocation_info	*r;
	int	length;

	for (r=Relocation; r<&Relocation[Relsize]; r++) {
		if (r->r_address == dot-4)
			return(r->r_symbolnum);
	}
	return(-1);
}

printins(inst)
{	
	register struct opdesc *p;
	register int (*fun)();

	/* this messes up "orb" instructions ever so slightly, but keeps
		 * us in sync between routines...
		 */
	if (inst == 0) {
		addtext( "\t.word 0");
		return;
	}

	for (p = opdecode; p->mask; p++)
		if ((inst & p->mask) == p->match) break;

	if (p->mask != 0) (*p->opfun)(inst, p->farg);
	else {
		addtext( badop);
	}
}

long sxtword(i)
register int	i;
{
	return( i>32767 ? i-65536 : i );
}

long instfetch(size)
register int size;
{	
	register int ans = 0;
	char	string[4];

	while (size--) {
		ans <<= 8;
		ans |= getc(infile) & 0377;
		sprintf(string, "%2.2X", ans&0377);
		strcat(objcode,string);
		dot++;
	}

	strcat(objcode," ");
	return(ans);
}

printea(mode,reg,size)
long mode, reg;
int size;
{
	long index,disp;

	switch ((int)(mode)) {
	case 0:	
		addtext( "d%D",reg);
		break;

	case 1:	
		addtext("a%D",reg);
		break;

	case 2:	
		addtext( "a%D@",reg);
		break;

	case 3:	
		addtext( "a%D@+",reg);
		break;

	case 4:	
		addtext( "a%D@-",reg);
		break;

	case 5:	
		addtext( "a%D@(%D)",reg,sxtword(instfetch(2)));
		break;

	case 6:	
		index = instfetch(2);
		disp = (char)(index&0377);
		addtext("a%d@(%d,%c%D%c)",reg,disp,
		(index&0100000)?'a':'d',(index>>12)&07,
		(index&04000)?'l':'w');
		break;

	case 7:	
		switch ((int)(reg))
		{
		case 0:	
			index = instfetch(2);
			addtext( "%x:w",index);
			break;

		case 1:	
			index = instfetch(4);
			pexact(index, 0);
			break;

		case 2:	
			addtext("pc@(%D)",sxtword(instfetch(2)));
			break;

		case 3:	
			index = instfetch(2);
			disp = (char)(index&0377);
			addtext( "a%D@(%D,%c%D:%c)",reg,disp,
			(index&0100000)?'a':'d',(index>>12)&07,
			(index&04000)?'l':'w');
			break;

		case 4:	
			index = instfetch(size==4?4:2);
			addtext( IMDF, index);
			break;

		default:	
			addtext( "???");
			break;
		}
		break;

	default:	
		addtext( "???");
	}
}

printEA(ea,size)
long ea;
int size;
{
	printea((ea>>3)&07,ea&07,size);
}

mapsize(inst)
long inst;
{
	inst >>= 6;
	inst &= 03;
	return((inst==0) ? 1 : (inst==1) ? 2 : (inst==2) ? 4 : -1);
}

char suffix(size)
register int size;
{
	return((size==1) ? 'b' : (size==2) ? 'w' : (size==4) ? 'l' : '?');
}

omove(inst, s)
long inst;
char *s;
{
	int size;

	addtext( "\tmov%c\t",*s);
	size = ((*s == 'b') ? 1 : (*s == 'w') ? 2 : 4);
	printEA(inst, size);
	strcat(text, ",");
	printea((inst>>6)&07,(inst>>9)&07,size);
}

obranch(inst,dummy)
long inst;
{
	long disp = inst & 0377;
	char *s; 

	s = "s ";
	if (disp > 127) disp |= ~0377;
	else if (disp == 0) { 
		s = " "; 
		disp = sxtword(instfetch(2))-2; 
	}
	addtext("\tb%s%s\t",bname[(int)((inst>>8)&017)],s);
	addtext("0x%x",dot+disp);
}

oscc(inst,dummy)
long inst;
{
	addtext( "\ts%s\t",ccname[(int)((inst>>8)&017)]);
	printEA(inst, 1);
}

odbcc(inst,dummy)
long inst;
{
	int	const;

	const = instfetch(2);
	addtext( "\tdb%s\t", ccname[(int)((inst>>8)&017)] );
	printea(0, inst & 07, 1);
	strcat(text, ",");
	pexact(dot+sxtword(const), N_TEXT); 
}

biti(inst, dummy)
long inst;
{
	addtext( "\t%s\t", bit[(int)((inst>>6)&03)]);
	if (inst&0x0100) {
		addtext( "d%D,", inst>>9);
	} else { 
		addtext( IMDF, instfetch(2)); 
	}
	strcat(text, ",");
	printEA(inst);
}

opmode(inst,opcode)
long inst;
{
	register int opmode = (int)((inst>>6) & 07);
	register int reg = (int)((inst>>9) & 07);
	int size;

	size = (opmode==0 || opmode==4) ?
	1 : (opmode==1 || opmode==3 || opmode==5) ? 2 : 4;
	addtext( "\t%s%c\t", opcode, suffix(size));
	if (opmode>=4 && opmode<=6)
	{
		addtext("d%d,",reg);
		printEA(inst, size);
	}
	else
	{
		printea((inst>>3)&07,inst&07, size);
		addtext(",%c%d",(opmode<=2)?'d':'a',reg);
	}
}

shroi(inst,ds)
long inst;
char *ds;
{
	int rx, ry;
	char *opcode;
	if ((inst & 0xC0) == 0xC0)
	{
		opcode = shro[(int)((inst>>9)&03)];
		addtext( "\t%s%s\t", opcode, ds);
		printEA(inst);
	}
	else
	{
		opcode = shro[(int)((inst>>3)&03)];
		addtext( "\t%s%s%c\t",opcode, ds, suffix(mapsize(inst)));
		rx = (int)((inst>>9)&07); 
		ry = (int)(inst&07);
		if ((inst>>5)&01) {
			addtext( "d%d,d%d", rx, ry);
		} else {
			addtext( IMDF, (rx ? rx : 8));
			addtext( ",d%d", ry);
		}
	}
}		

oimmed(inst,opcode) 
long inst;
register char *opcode;
{
	register int size = mapsize(inst);
	long const;

	if (size > 0)
	{
		const = instfetch(size==4?4:2);
		addtext( "\t%s%c\t", opcode, suffix(size));
		addtext( IMDF, const); 
		strcat(text, ",");
		printEA(inst,size);
	}
	else {
		addtext(badop, NULL);
	}
}

oreg(inst,opcode)
long inst;
register char *opcode;
{
	addtext( opcode, (inst & 07));
}

extend(inst, opcode)
long	inst;
char	*opcode;
{
	register int size = mapsize(inst);
	int ry = (inst&07), rx = ((inst>>9)&07);
	char c;

	c = ((inst & 0x1000) ? suffix(size) : ' ');
	addtext( "\t%s%c\t", opcode, c);
	if (*opcode == 'e')
	{
		if (inst & 0x0080) {
			addtext( "d%D,a%D", rx, ry);
		}
		else if (inst & 0x0008) {
			addtext( "a%D,a%D", rx, ry);
		} else {
			addtext( "d%D,d%D", rx, ry);
		}
	} else if ((inst & 0xF000) == 0xB000) {
		addtext( "a%D@+,a%D@+", ry, rx);
	} else if (inst & 0x8) {
		addtext( "a%D@-,a%D@-", ry, rx);
	} else {
		addtext( "d%D,d%D", ry, rx);
	}
}


olink(inst,dummy)
long inst;
{
	addtext( "\tlink\ta%D,", inst&07);
	addtext( IMDF, sxtword(instfetch(2)));
}

otrap(inst,dummy)
long inst;
{
	addtext( "\ttrap\t");
	addtext( IMDF, inst&017);
}

oneop(inst,opcode)
long inst;
register char *opcode;
{
	addtext("\t%s\t",opcode);
	printEA(inst);
}

omoveto(inst, opcode)
long	inst;
register char	*opcode;
{
	addtext("\tmove\t");
	printEA(inst);
	addtext(",%s", opcode);
}

omovefrom(inst, opcode)
long	inst;
register char	*opcode;
{
	addtext("\tmove\t%s,", opcode);
	printEA(inst);
}

pregmask(mask)
register int mask;
{
	register int i;
	register int flag = 0;

	addtext("#<");
	for (i=0; i<16; i++)
	{
		if (mask&1)
		{
			if (flag) 
				strcat(text, ",");
			else flag++;
			addtext("%c%d",(i<8)?'d':'a',i&07);
		}
		mask >>= 1;
	}
	addtext(">");
}

omovem(inst,dummy)
long inst;
{
	register int i, list = 0, mask = 0100000;
	register int reglist = (int)(instfetch(2));

	if ((inst & 070) == 040)	/* predecrement */
	{
		for(i = 15; i > 0; i -= 2)
		{ 
			list |= ((mask & reglist) >> i); 
			mask >>= 1; 
		}
		for(i = 1; i < 16; i += 2)
		{ 
			list |= ((mask & reglist) << i); 
			mask >>= 1; 
		}
		reglist = list;
	}
	addtext("\tmovem%c\t",(inst&100)?'l':'w');
	if (inst&02000)
	{
		printEA(inst);
		strcat(text, ",");
		pregmask(reglist);
	}
	else
	{
		pregmask(reglist);
		strcat(text, ",");
		printEA(inst);
	}
}

ochk(inst,opcode)
long inst;
register char *opcode;
{
	addtext("\t%s\t",opcode);
	printEA(inst);
	addtext(",%c%D",(*opcode=='l')?'a':'d',(inst>>9)&07);
}

soneop(inst,opcode)
long inst;
register char *opcode;
{
	register int size = mapsize(inst);

	if (size > 0)
	{
		addtext("\t%s%c\t",opcode,suffix(size));
		printEA(inst);
	}
	else 
		addtext(badop, NULL);
}

oquick(inst,opcode)
long inst;
register char *opcode;
{
	register int size = mapsize(inst);
	register int data = (int)((inst>>9) & 07);

	if (data == 0) data = 8;
	if (size > 0)
	{
		addtext("\t%s%c\t", opcode, suffix(size));
		addtext(IMDF, data); 
		strcat(text, ",");
		printEA(inst);
	}
	else addtext(badop, NULL);
}

omoveq(inst,dummy)
long inst;
{
	register int data = (int)(inst & 0377);

	if (data > 127) data |= ~0377;
	addtext("\tmoveq\t"); 
	addtext(IMDF, data);
	addtext(",d%D", (inst>>9)&07);
}

oprint(inst,opcode)
long inst;
register char *opcode;
{
	addtext("\t%s",opcode);
}

addtext(control, arg1, arg2, arg3, arg4, arg5, arg6)
char	*control;
int	arg1, arg2, arg3, arg4, arg5, arg6;
{
	char	string[100];

	sprintf(string, control, arg1, arg2, arg3, arg4, arg5, arg6);
	strcat(text, string);
}

mystrlen(string)
char	*string;
{
	int	length = 0;
	while (*string++ != NULL) {
		if (*string == '\t')
			length += ( 7 - (length -1) % 8);
		else if (*string == '\n')
			length = 0;
		else length++;
	}
	return(length);
}
