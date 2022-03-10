struct	exec {	/* a.out header */
	short		a_magic;	/* magic number */
	unsigned short	a_text;		/* size of text segment */
	unsigned short	a_data;		/* size of initialized data */
	unsigned short	a_bss;		/* size of unitialized data */
	unsigned short	a_syms;		/* size of symbol table */
	unsigned short	a_entry;	/* entry point */
	unsigned short	a_unused;	/* not used */
	unsigned short	a_flag;		/* relocation info stripped */
};

#define	A_MAGIC1	0407       	/* normal */
#define	A_MAGIC2	0410       	/* read-only text */
#define	A_MAGIC3	0411       	/* separated I&D */
#define	A_MAGIC4	0405       	/* overlay */

struct	nlist {	/* symbol table entry */
	char		n_name[8];	/* symbol name */
	short		n_type;		/* type flag */
	unsigned short	n_value;	/* value */
};

		/* values for type flag */
#define	N_UNDF		0	/* undefined */
#define	N_ABS		01	/* absolute */
#define	N_TEXT		02	/* text symbol */
#define	N_DATA		03	/* data symbol */
#define	N_BSS		04	/* bss symbol */
#define	N_TYPE		037
#define	N_REG		024	/* register name */
#define	N_FN		037	/* file name symbol */
#define	N_EXT		040	/* external bit, or'ed in */
#define	FORMAT		"%06o"	/* to print a value */

		/* values for relocation words */
#define R_TYPE		016	/* mask to extract type */
#define R_ABS		0	/* abs value */
#define R_TEXT		02	/* text relocation */
#define R_DATA		04	/* data relocation */
#define R_BSS		06	/* bss relocation */
#define R_UNDF		010	/* undefined external relocation */
#define R_PCREL		01	/* bit for pc relative relocation */

#define STABTYPES	0	/* used by sdb on vax */


