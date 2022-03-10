/*	This file must be maintained in:
 *
 *		/trix/include/b.out.h
 *		/include/b.out.h
 *
 *	Layout of b.out file :
 *
 *	header of 8 longs	magic number 405, 407, 410, 411
 *				text size		)
 *				data size		) in bytes
 *				bss size		)
 *				symbol table size	)
 *				text relocation size	)
 *				data relocation size	)
 *				entry point
 *
 *
 *	header:			0
 *	text:			32
 *	data:			32+textsize
 *	symbol table:		32+textsize+datasize
 *	text relocation:	32+textsize+datasize+symsize
 *	data relocation:	32+textsize+datasize+symsize+rtextsize
 *
 */
/* various parameters */
#define SYMLENGTH	50		/* maximum length of a symbol */
#define PAGESIZE	1024		/* relocation boundry for 410 files */

/* types of files */
#define	ARCMAGIC 0177545
#ifndef OMAGIC
#define OMAGIC	0405
#endif
#define	FMAGIC	0407
#ifndef NMAGIC
#define	NMAGIC	0410
#endif
#define	IMAGIC	0411

/* symbol types */
#define	EXTERN	040
#define	UNDEF	00
#define	ABS	01
#define	TEXT	02
#define	DATA	03
#define	BSS	04
#define	COMM	05	/* internal use only */
#define REG	06

/* relocation regions */
#define	RTEXT	00
#define	RDATA	01
#define	RBSS	02
#define	REXT	03

/* relocation sizes */
#define RBYTE	00
#define RWORD	01
#define RLONG	02

/* macros which define various positions in file based on a bhdr, filhdr */
#define TEXTPOS		sizeof(filhdr)
#define DATAPOS 	TEXTPOS + filhdr.tsize
#define SYMPOS		DATAPOS + filhdr.dsize
#define RTEXTPOS	SYMPOS + filhdr.ssize
#define RDATAPOS	RTEXTPOS + filhdr.rtsize
#define ENDPOS		RDATAPOS + filhdr.rdsize
/* header of b.out files */
struct bhdr {
	long	fmagic;
	long	tsize;
	long	dsize;
	long	bsize;
	long	ssize;
	long	rtsize;
	long	rdsize;
	long	entry;
};

/* symbol management */
struct sym {
	char	stype;
	long	svalue;
};

/* relocation commands */
struct reloc {
	unsigned rsegment:2;	/* RTEXT, RDATA, RBSS, or REXTERN */
	unsigned rsize:2;		/* RBYTE, RWORD, or RLONG */
	unsigned rdisp:1;		/* 1 => a displacement */
	struct sym_bkt *rsymbol;		/* id of the symbol of external relocations */
	long rpos;		/* position of relocation in segment */
};
