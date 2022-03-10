#include <stdio.h>
#include <a.out.h>
#include <ctype.h>

struct nlist N;
struct relocation_info R;
struct exec H;
int addr;

FILE *ofile;
main(argc,argv)
char *argv[];
{
	if((ofile = fopen(argv[1],"r")) == NULL) {
		fprintf(stderr,"Can't open .o file\n");
		exit(1);
	}
	fread(&H,sizeof(H),1,ofile);
	printf("Magic=%o, text=%d, data=%d, bss=%d\n",H.a_magic,H.a_text,
		H.a_data,H.a_bss);
	printf("syms=%d, entry=%X, trsize=%d, drsize=%d\n",H.a_syms,H.a_entry,
		H.a_trsize,H.a_drsize);
	printf("TXTOFF=0%o,%d, SYMOFF=0%o,%d, STROFF=0%o,%d\n",N_TXTOFF(H),
		N_TXTOFF(H),N_SYMOFF(H),N_SYMOFF(H),N_STROFF(H),N_STROFF(H));
	addr = H.a_entry;
	printf("\n		Text Segment\n\n");
	fseek(ofile,N_TXTOFF(H),0);
	shobytes(H.a_text,0);
	printf("\n		Data Segment\n\n");
	shobytes(H.a_data,1);
	printf("\n		Text Relocation\n\n");
	shoreloc(H.a_trsize);
	printf("\n		Data Relocation\n\n");
	shoreloc(H.a_drsize);
	printf("\n		Symbol table\n\n");
	shosyms(H.a_syms);
	printf("\n		Strings\n\n");
	shostr();
}

shobytes(n,printable)
{
	register i;
	unsigned char c;

	for(i=0;i<n;i++) {
		if((i%16)==0)
			printf("\n %6.6X	",addr);
		addr++;
		fread(&c,sizeof(c),1,ofile);
		if (printable && isprint(c))
			printf("%2.2c ",c);
		else
			printf("%2.2X ",c);
	}
	printf("\n");
}

shoreloc(n)
{
	struct relocation_info r;
	int i=0;
	printf("count  addr   symnum   pcrel   length  extern\n");
	while(n) {
		fread(&r,sizeof(r),1,ofile);
		printf("%d:     %d,      %d,      %d,      %d,      %d\n",
			i++,r.r_address,r.r_symbolnum,r.r_pcrel,r.r_length,r.r_extern);
		n -= sizeof(r);
	}
}

shosyms(n)
{
	struct nlist s;
	int i=0;

	while(n) {
		fread(&s,sizeof(s),1,ofile);
		printf("%d:  %d, %d, %d, %d, %X\n",i++,s.n_un.n_strx,s.n_type,s.n_other,
			s.n_desc,s.n_value);
		n -= sizeof(s);
	}
}

shostr()
{
	int length;
	unsigned char c;
	int i=4;   /* Starts after length */

	fread(&length,sizeof(length),1,ofile);
	length -= 4; /* Bump past length */
	printf("String table length=%d\n4:	",length);
	while(length--){
		i++;
		fread(&c,sizeof(c),1,ofile);
		if(c)
			printf("%c",c);
		else
			printf("\n%d:	",i);
	}
	printf("\n");
}
