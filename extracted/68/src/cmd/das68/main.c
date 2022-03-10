#include "define.h"
#include <a.out.h>

/* dump text portion of .o file as instructions and symbols ... */

long	dot;
FILE	*infile;
char	objcode[100];
char	text[100];
char	blank40[] = "                                        ";
struct	exec	a;

char *ccname[16] = { 
	"t", "f", "hi", "ls", "cc", "cs", "ne",
	"eq", "vc", "vs", "pl", "mi", "ge", "lt", "gt", "le" };

/*------------------------------------------------------------------------------
 *  main  --  mc68000 dis-assembler
 *------------------------------------------------------------------------------
 */
main(argc,argv)
int	argc;
char	*argv[];
{

	setbuf(stdout, NULL);
	if (argc == 2) {
		if ((infile = fopen(argv[1],"r")) == NULL) {
			fprintf(stderr,"can't open %s for input\n",argv[1]);
			exit(1);
		}
	} 
	else infile = stdin;

	GetHeader();
	GetRelocationInfo();
	GetSymbolTable();
	GetStrings();
		
	fseek(infile, N_TXTOFF(a), 0);		/* Seek back to text segment */
	dot = a.a_entry;
	while (dot < a.a_text+a.a_entry) {
		objcode[0] = '\0';
		text[0] = '\0';
		pexact(dot, 0);
		addtext(":");
		if( mystrlen(text) < 8)
			addtext("\t");
		printins(instfetch(2));
		strncat(text, blank40, 40 - mystrlen(text));
		strcat(text, objcode);
		printf("%s\n", text);
	}
	exit(0);
}
