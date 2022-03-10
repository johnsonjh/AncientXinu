#include <a.out.h>
#include "mical.h"
#include "scan.h" /* YIP */

/* Allocation increments for symbol buckets and character blocks */
#define	SYM_INCR	50
#define CBLOCK_INCR	512

extern struct stab_sym_bkt *stabkt_head; /* YIP */

struct sym_bkt *Last_symbol;			/* last symbol defined */
struct sym_bkt *sym_hash_tab[HASH_MAX];		/* Symbol hash table */
struct sym_bkt *sym_free = NULL;		/* head of free list */
char *cblock = NULL;				/* storage for symbol names */
int ccnt = 0;					/* number of chars left in c block */

/* grab a new symbol bucket off of the free list; allocate space
 * for a new free list if necessary
 */
struct sym_bkt *gsbkt()
  {	register struct sym_bkt	*sbp;
	register int i;

	if ((sbp = sym_free) != NULL) sym_free = sbp->next_s;
	else {
	  sbp = (struct sym_bkt *)calloc(SYM_INCR,sizeof(struct sym_bkt));
	  if (sbp == NULL) Sys_Error("Symbol storage exceeded\n",0);
	  for (i = SYM_INCR-1; i--;) {
	    sbp->next_s = sym_free;
	    sym_free = sbp++;
	  }
	}

	return(sbp);
}

/* initialize hash table */
Sym_Init()
  {	register int i;

	for (i=0; i<HASH_MAX; i++) sym_hash_tab[i] = NULL;
} /* end Sym_Init */

char *sstring(string)
  register char *string;
  {	register char *p,*q;	/* working char string */
	register int i;

	i = strlen(string);	/* get length of string */

	if (++i > ccnt) {	/* if not enough room get more */
	  if ((cblock = (char *)calloc(CBLOCK_INCR,1)) == NULL)
	    Sys_Error("Symbol storage exceeded\n",0);
	  ccnt = CBLOCK_INCR;
	}

	p = q = cblock;		/* copy string into permanent storage */
	while (*p++ = *string++);
	cblock = p;
	ccnt -= i;
	return(q);
} /* end sstring */

/* lookup symbol in symbol table */
struct sym_bkt *Lookup(s)
register char *s;
  {	register struct sym_bkt	*sbp;	/* general purpose ptr */
	register int Save;		/* save subscript in sym_hash_tab */
	register char *p;
	char local[50];			/* used for constructing local sym */

	if (*s>='0' && *s<='9') {	/* local symbol hackery */
	  p = local;
	  while (*p++ = *s++);		/* copy local symbol */
	  p--;
	  s = Last_symbol->name_s;	/* add last symbol defined as suffix */
	  while (*p++ = *s++);
	  s = local;			/* this becomes name to deal with */
	}

	/* if the symbol is already in here, return a ptr to it */
	for (sbp = sym_hash_tab[Save=Hash(s)]; sbp != NULL ; sbp = sbp->next_s)
	  if (strcmp(sbp->name_s,s) == 0) return(sbp);

	/* Since it's not, make a bucket for it, and put the bucket in the symbol table */
	sbp = gsbkt();				/* get the bucket */
	sbp->name_s = sstring(s);		/* Store it's name */
	sbp->value_s = sbp->id_s = sbp->attr_s = 0;
	sbp->csect_s = NULL;
	sbp->next_s = sym_hash_tab[Save];	/* and insert on top of list */
	if (s == local) sbp->attr_s |= S_LOCAL;
	return(sym_hash_tab[Save] = sbp);
}

/* Sym_Fix - Assigns index numbers to the symbols.  Also performs 
             relocation of the symbols assuming data segment follows 
             text and bss follows the data.  If global flag, make all 
             undefined symbols defined to be externals.
*/
Sym_Fix()
{
  register struct sym_bkt **sbp1, *sbp2;
  int i = 0;

  for (sbp1 = sym_hash_tab; sbp1 < &sym_hash_tab[HASH_MAX]; sbp1++)
      for (sbp2 = *sbp1; sbp2; sbp2 = sbp2->next_s) 
          { if ((sbp2->attr_s & (S_DEC|S_DEF)) == 0)
	       { sbp2->attr_s |= S_EXT | S_DEC;
	         sbp2->csect_s = NULL;
	       }
	    sbp2->value_s += sdi_inc(sbp2->csect_s, sbp2->value_s);
	    if (sbp2->csect_s == Data_csect) 
               sbp2->value_s += tsize;
	    else if (sbp2->csect_s == Data1_csect) 
                    sbp2->value_s += tsize + dsize;
	         else if (sbp2->csect_s == Data2_csect) 
                         sbp2->value_s += tsize + dsize + d1size;
	              else if (sbp2->csect_s == Bss_csect) 
                              sbp2->value_s += tsize + dsize + d1size + d2size;
	    if (sbp2==Dot_bkt || sbp2->attr_s & (S_REG|S_MACRO|S_LOCAL|S_PERM))
	       sbp2->id_s = -1;
	    else sbp2->id_s = i++;
	  }
}


/* Sym_Write -	Write out the symbols to the specified
		file in b.out format, while computing size
		of the symbol segment in output file.
 */

redosyms()
{
	/* Go through the symbol table and get rid of "L" syms if 
		we are supposed to. */
	long size = 0;
	register struct sym_bkt  **sbp1, *sbp2;
	register char *sp;
	int slength;
	struct nlist s;
	register strcount = 4;


	for (sbp1 = sym_hash_tab; sbp1 < &sym_hash_tab[HASH_MAX]; sbp1++)
		if (sbp2 = *sbp1) for (; sbp2; sbp2 = sbp2->next_s)
		{
			if (sbp2->id_s != -1) 
			if (chkname(sbp2->name_s))
				sbp2->final = size++;
		}
}

long Sym_Write(file)
  FILE *file;
  { register struct sym_bkt  **sbp1, *sbp2;
    register char *sp;
    long size = 0;
    struct nlist s;
    int strcount = 4;
    struct stab_sym_bkt *t;
	
    for (sbp1 = sym_hash_tab; sbp1 < &sym_hash_tab[HASH_MAX]; sbp1++)
        if (sbp2 = *sbp1) for (; sbp2; sbp2 = sbp2->next_s)
	   if (sbp2->id_s != -1) 
	      if (chkname(sbp2->name_s)) {
		 /* Write out the symbol table using the VAX a.out format */
		 if ((sbp2->attr_s&S_DEF)== 0)s.n_type = N_UNDF;
		 else if (sbp2->csect_s == Text_csect) s.n_type = N_TEXT;
		      else if (sbp2->csect_s == Data_csect) s.n_type = N_DATA;
			   else if (sbp2->csect_s == Data1_csect) s.n_type = N_DATA;
				else if (sbp2->csect_s == Data2_csect) s.n_type = N_DATA;
				     else if (sbp2->csect_s == Bss_csect) s.n_type = N_BSS;
				          else s.n_type = N_ABS;
				if (sbp2->attr_s & S_EXT) s.n_type |= N_EXT;
				s.n_value = sbp2->value_s;
				/* For right now, just stuff these with 0 */
				s.n_other = s.n_desc = 0;
				s.n_un.n_strx = strcount;
				strcount += strlen(sbp2->name_s)+1;
				size += sizeof(s);
				fwrite(&s,sizeof(s),1,file);
			}

        /* This is to write out the .stabs and .stabd symbols onto the        */
        /* a.out file.  This is only being written after the regular          */
        /* symbols have been put out (the prior section of the function).     */
        t = stabkt_head;           /* obtain head of stabs/stabd symbol table */
        while (t != NULL)
              { s.n_type   = t->type;
                s.n_other  = t->other;
                s.n_desc   = t->desc;       
                s.n_value  = t->value;                /* zero for testing now */
                if (t->id == 1)
                   { s.n_un.n_strx = strcount;      /* assign string offset   */
                     strcount += strlen(t->ch) + 1; /* increment str location */
                   }                                /* in string table.       */
                else s.n_un.n_strx = 0;             /* else if no string is   */
                                                    /* present, assign 0.     */
                size += sizeof(s);
                fwrite(&s, sizeof(s), 1, file);
                t = t->next_stab;
              } /* end while */
	return(size);
}

long Str_Write(file)
FILE *file;
{
	long size = 0;
	register struct sym_bkt  **sbp1, *sbp2;
	register char *sp;
	int slength;
	struct nlist s;
	int strcount = 4;
        struct stab_sym_bkt *t;


	fwrite(&strcount,sizeof(long),1,file);
	for (sbp1 = sym_hash_tab; sbp1 < &sym_hash_tab[HASH_MAX]; sbp1++)
		if (sbp2 = *sbp1) for (; sbp2; sbp2 = sbp2->next_s)
		if (chkname(sbp2->name_s))
		{
			if (sbp2->id_s != -1) {
				register i;
				strcount +=  i = strlen(sbp2->name_s)+1;
				fwrite(sbp2->name_s,i,1,file);
			}
		}

        /* This is to write out all the symbols (strings) generated by        */
        /* .stabs and .stabd directives.  They are being written onto the     */
        /* string table only after all the regular symbol tables have been    */
        /* written out (by the prior section of this function.                */
        t = stabkt_head;                     /* head of stabs/stabd link list */
        while (t != NULL)
              { register i;
                if (t->id == 1)
                   { strcount += i = strlen(t->ch) + 1;
                     fwrite(t->ch,i,1,file); 
                   }
                t = t->next_stab;
              } /* end while */
	return(strcount);
}

extern char O_Lflag;
chkname(name)
char *name;
{
	if(O_Lflag||(*name!='L')) return(1);
	return(0);
}

/*
 * Perm	Flags all currently defined symbols as permanent (and therefore
 *	ineligible for redefinition.  Also prevents them from being output
 *	in the object file).
 */
Perm()
  {	register struct sym_bkt **sbp1, *sbp2;

	for (sbp1 = sym_hash_tab; sbp1 < &sym_hash_tab[HASH_MAX]; sbp1++)
		for (sbp2 = *sbp1; sbp2; sbp2 = sbp2->next_s)
			sbp2->attr_s |= S_PERM;
}
