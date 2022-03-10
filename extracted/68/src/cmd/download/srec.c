/* srec.c - srec */

#include <stdio.h>
#include <a.out.h>
#define	NPL	10

struct	exec	hdr;
int	cksum;
FILE	*fi;

char	buf[BUFSIZ];

/*-----------------------------------------------------------------------------
  *  srec -- 	PM68K 'S' - record generator  
  * We take a a.out format file, read the information about the length of the
  * text segment, skip past the remainder of the header and read the code.
  * Each 's' record consists of an 'S' a number (either 2 or 8 in our case)
  * the length of the record, the address to place the first byte of the 's'
  * record and a checksum.  The version presented here assumes a text segment
  * origin of 0x2000.  This was done to avoid conflicts with the SUN's rom
  * monitor routines which occupy the first 8k of the SUN workstation's
  * memory. The 68k code is placed in records of the format described above
  * with a number 2. Then after all the code has been converted to these
  * records, a final record is created with a number of 8.  This record
  * contains the starting address of the code in the spot where the loading
  * address would have been.  Upon recieving this record the SUN will
  * execute the code.
  *--------------------------------------------------------------------------
  */


main(argc, argv)
int	argc;
char	**argv;
{
	long	addr;
	long	start_addr;
	long	lim;
	register	w;

	if (argc<=1)
		exit(0);
	setbuf(stdout, buf);
	fi = fopen(argv[1], "r");
	if (fi==NULL) {
		fprintf(stderr, "In file?\n");
		exit(1);
	}
	fread((char *)&hdr, sizeof(struct exec), 1, fi);
	start_addr = 0x1000; 
	addr = start_addr;
	lim = addr + hdr.a_text + hdr.a_data;
	fseek(fi, N_TXTOFF(hdr), 0);
	while (addr < lim) {
		cksum = 0;
		printf("S2");
		prhex(1, (long)(4+2*NPL));
		prhex(3, addr);
		for (w=0; w<NPL; w++)
			prwrd();
		prhex(1, (long)~cksum);
		printf("\r\n");
		addr += 2*NPL;
	}
	cksum = 0;
	printf("S8");
	prhex(1, (long)4);
	prhex(3, hdr.a_entry);
	prhex(1, (long)~cksum);
	printf("\r\n");
}

prhex(n, v)
long v;
{
	register i, d;

	for (i=n-1; i>=0; i--) {
		d = v>>(i*8);
		cksum += d;
		xdig(d);
	}
}

xdig(d)
{
	register c;

	c = (d>>4) & 0xf;
	if (c<=9)
		putchar(c+'0');
	else
		putchar(c-0xa+'A');
	c = d&0xf;
	if (c<=9)
		putchar(c+'0');
	else
		putchar(c-0xa+'A');
}

 /* The major problem with creating 's' records is the switching of the order
  * of bytes required.  This took forever to figure out! */
 
prwrd()
{
	register w;
	register x, y, z;
	w = getc(fi) << 8;
	w |= getc(fi);
	prhex(2, (long)w);
}
