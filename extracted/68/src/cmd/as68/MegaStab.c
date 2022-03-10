#include "scan.h" 
#include "mical.h"

/* extern short cinfo[];                    */
/* extern struct stab_sym_bkt *stabkt_head; */
/* extern struct stab_sym_bkt *stabkt_tail; */
/* extern struct csect *Cur_csect; ........ */ 
/* ..... added in for 2nd pass dot location */
/* extern struct sym_bkt *Last_symbol;      */
extern struct sym_bkt *sym_hash_tab[];
extern long Dot;                       /* current offset in csect (i.e. addr)*/

#define Stab_NULL        0
#define MISSING_STRING   1
#define MISSING_QUOTE    2
#define ILLEGAL_HEX      3
#define ILLEGAL_OCTAL    4
#define ILLEGAL_DECIMAL  5
#define STABFLOATING     0
#define STABFIXED        1

#define NL     012            /* New Line character */
#define COMMA  054            /* ","                */
#define DQUOTE 042            /* """                */

/* #define skips(p) while (cinfo[*p] & T) p++ */
/* #define skipb(p) while (cinfo[*p] == SPC) p++ */

char *Stab_Messages[] = {
     /* 0 */    "", /* not used */
     /* 1 */    "Expecting a string in .stabs directive.", 
     /* 2 */    "At least 1 quote mark is missing from string.",
     /* 3 */    "Illegal hexadecimal digit found.",
     /* 4 */    "Illegal octal digit found.",
     /* 5 */    "Illegal decimal digit found.",
                0                   /* YIP */
};


StabStr(opcode, operand)
char *opcode;
char *operand;

{ struct stab_sym_bkt *s;          /* ptr to new allocated mem for stab bkt */
  char *symbol;
  char *c;
  int StrLen;                      /* String Length */

  /* Allocate memory space to stab symbol */
  /* table for incoming stab directive */
  s = (struct stab_sym_bkt *) malloc(sizeof (struct stab_sym_bkt));
  if (s == Stab_NULL)
     Sys_Error("Stab Symbol storage exceeded.\n",0);
 
  /* Obtain different fields from the .stabs directive */
  s->id = 1;                               
  StrLen = 0;
  if (*operand != DQUOTE)                 
     Sys_Error("Expected a string in .stabs directive.\n",0);

  operand++;                             
  symbol = operand;                     
  while ((*operand != DQUOTE) && (*operand != Stab_NULL))
        { operand++;                                    
          StrLen++;                      
        }
  if (*operand == Stab_NULL)
     Sys_Error("No ending quote for string in .stabs directive.\n",0);
  s->ch = (char *) malloc(StrLen + 1); 
  strncpy(s->ch, symbol, StrLen);    
  *(s->ch + StrLen) = Stab_NULL;           /* put a "\0" at end of string */
  
  while ((*operand != COMMA) && (*operand != Stab_NULL)) 
        operand++;                      
  if (*operand == Stab_NULL)
     Sys_Error("Unfinished .stabs directive.\n",0);
  else operand++;                  
  while (*operand == Stab_NULL)
	operand++;

  symbol = operand;
  s->type  = Get_Token_Value(symbol);            

  skips(operand); skipb(operand);               
  if (*operand == COMMA)
     operand++;
  symbol = operand;
  s->other = Get_Token_Value(symbol);          

  skips(operand); skipb(operand);             
  if (*operand == COMMA)
     operand++;
  symbol = operand;
  s->desc  = Get_Token_Value(symbol);        

  skips(operand); skipb(operand);            
  if (*operand == COMMA)
     operand++;
  symbol = operand;                         
  if ((s->type == 0x24) || (s->type == 0x26) || (s->type == 0x64))
     {  StrLen = 0;
        while ((*operand != Stab_NULL) && (*operand != NL))
              { operand++; 
                StrLen++;
              }
        s->label = (char *) malloc(StrLen + 1);
        strncpy(s->label, symbol, StrLen);
        s->tag = STABFLOATING;
        
     }
  else { s->value =  Get_Token_Value(symbol); 
         s->tag = STABFIXED;
       }

  Link_Stab_Tab(s);

} /* end StabStr */

/* ************************************************************************** */

StabDot(token, operand)
char *token; 
char *operand;

{ struct stab_sym_bkt *s;
  char *symbol;
  
  s = (struct stab_sym_bkt *) malloc(sizeof(struct stab_sym_bkt));
  if (s == Stab_NULL)
     Sys_Error("Stab Symbol storage exceeded.\n",0);

  s->id = 0;                                        /* 0 = no string name */
  s->tag = STABFIXED;
  /* Obtain different fields for the .stabd directive */

  symbol = operand;
  skips(operand); skipb(operand);
  if (*operand == COMMA)
     operand++;
  s->type  = Get_Token_Value(symbol);               /* get field "type"   */

  symbol = operand;                                 /* save current token */
  skips(operand); skipb(operand);                   /* skip to next field */
  if (*operand == COMMA)
     operand++;
  s->other = Get_Token_Value(symbol);               /* get field "other"  */

  symbol = operand;                                 /* save current token */
  skips(operand); skipb(operand);                   /* skip to next field */
  if (*symbol == COMMA)
     operand++;
  s->desc  = Get_Token_Value(symbol);               /* get field "desc"   */

  s->value = Dot;                                   /* Dot = current addr */

  Link_Stab_Tab(s);

} /* end StabDot */

/* ************************************************************************* */

  /* Link the incoming stab bucket to the stab symbol table.       */
  /* The addition of the incoming stab is linked to the end of the */
  /* current linked list.                                          */


Link_Stab_Tab(s)
struct stab_sym_bkt *s;


{ if (stabkt_head == Stab_NULL)                 /* if no entry on linked list */
     { stabkt_head = s;                         /* let both head and tail     */
       stabkt_tail = s;                         /* pointing to this entry s   */
       stabkt_head->next_stab = Stab_NULL;      /* and make the next one null */
     }
     else { stabkt_tail->next_stab = s;          /* otherwise make this entry */
            stabkt_tail = stabkt_tail->next_stab; /* the next entry and move  */
            stabkt_tail->next_stab = Stab_NULL;  /* tail pointing to this one */
          }
} /* end Link_Stab_Tab */

/* ************************************************************************** */

/* This is the error message routine.  The program will abort after  */
/* the appropriate message is printed.                               */

Stab_Error(n)
int n;

{ printf("Error: %s \n", Stab_Messages[n]);
  printf("Program borted.");
  abort();
} /* end Stab_Error */

/* ************************************************************************** */

/* Convert_to_N converts a constant from the input line buffer buf[]       */
/* into its numeric value.  It first checks to see if it is a hexadecimal  */
/* constant, or an octal constant.  If neither one are true, decimal       */
/* is assumed.                                                             */

int Convert_to_N(Sbuf, Len)
char Sbuf[];
int Len;

{ int base;                           /* base of the number to be converted */
  int startentry; 
  int sum = 0;                        /* numeric value of the number        */
  int i;

  if ((Sbuf[0] == '0') && (Sbuf[1] == 'x'))      /* or a hexadecimal number */
     { startentry = 2;
       base = 16;
       Check_Hex(Sbuf, Len, startentry);           /* check legal hex digit */
     } 
     else if (Sbuf[0] == '0')
             { if (Len == 0)                              /* if only a zero */
                  return(0);
               startentry = 1;                              /* octal number */
               base = 8;
               Check_Octal(Sbuf, Len, startentry);
             }
          else { startentry = 0;                         /* decimal assumed */
                 base = 10;
                 Check_Decimal(Sbuf, Len, startentry);
               }
  for (i = startentry; i <= Len; i++)         /* obtain numeric value */
      sum = sum * base + Sbuf[i];
  return(sum);
} /* end Convert_to_N */

/* ************************************************************************ */

/* Get_Token_Value obtains the numeric values for the sub-fields type, other  */
/* and desc.                                                                  */
int Get_Token_Value(p)
char *p;

{ char *ch, *savech;
  int i, numeric_value;
  int last;
  char tokbuf[50];
  extern short cinfo[];

  for (i = 0; i <= 50; i++)
      tokbuf[i] = ' ';
  /* if (*p == COMMA)
     p++;
   */
  
  i = 0;
  while ((*p != COMMA) && (*p != Stab_NULL))       /* check end of token */
        { tokbuf[i] = *p;                        /* store into tokbuf for */
          p++; i++;                              /* numeric value calculation */
        }
  if ((*p != COMMA) && (*p != Stab_NULL))
     Sys_Error("Unknown end of token.\n",0);

  if (*p == Stab_NULL)                          /* obtain index of last digit */
     last = i - 2;
  else last = --i;

  numeric_value = Convert_to_N(tokbuf, last);
  return(numeric_value);

} /* end Get_Token_Value */

/* ************************************************************************** */

/* Check_Hex is to check the individual digits in the hex constant to see   */
/* if they are legal hex digits.  If so, they are converted to their        */
/* numeric value.                                                           */

Check_Hex(Sbuf, Len, start)
char Sbuf[];
int Len, start;

{ int i;

  for (i = start; i <= Len; i++)
      { if ((Sbuf[i] >= '0' && Sbuf[i] <= '9'))
           Sbuf[i] = Sbuf[i] - '0';
        else { switch (Sbuf[i])
               { case 'a' : 
                 case 'A' : { Sbuf[i] = 10; break; }
                 case 'b' :
                 case 'B' : { Sbuf[i] = 11; break; } 
                 case 'c' :
                 case 'C' : { Sbuf[i] = 12; break; }
                 case 'd' :
                 case 'D' : { Sbuf[i] = 13; break; }
                 case 'e' :
                 case 'E' : { Sbuf[i] = 14; break; }
                 case 'f' :
                 case 'F' : { Sbuf[i] = 15; break; }
                 default  : { printf("\nError in HEX converting...\n");
                              printf("S[%d] = %c\n", i, Sbuf[i]);
                              Stab_Error(ILLEGAL_HEX);
                            }
               } /* switch */
             } /* end else */
      } /* end if */
} /* end Check_Hex */

/* ************************************************************************ */

/* Check_Octal is to see if the individual digits in the constant are legal */
/* octal digits.  If so, they are converted to their numeric values.        */

Check_Octal(Sbuf, Len, start)
char Sbuf[]; 
int Len, start;

{ int i;

  for (i = start; i <= Len; i++)
      { if ((Sbuf[i] >= '0') && (Sbuf[i] <= '7'))
           Sbuf[i] = Sbuf[i] - '0';
        else { printf("\n ***** Last digit checked = %dth digit", i);
               printf("\n ***** ...digit = %c", Sbuf[i]);
               Stab_Error(ILLEGAL_OCTAL);
             }
      }
} /* end Check_Octal */

/* ************************************************************************ */

/* Check_Decimal is to see if the individual digits in the constant are     */
/* legal decimal digits.  If so, they are converted to their numeric values. */

Check_Decimal(Sbuf, Len, start) 
char Sbuf[];
int Len, start;

{ int i, j;

  for (i = start; i <= Len; i++)
      { if ((Sbuf[i] >= '0') && (Sbuf[i] <= '9'))
           Sbuf[i] = Sbuf[i] - '0';
        else { printf("\n Error : Sbuf[%d] = %c \n", i, Sbuf[i]); 
               printf("\n Sbuf[] = ");
               for (j = start; j <= Len; j++)
                   printf("%c", Sbuf[j]);
               printf("\n");
               Stab_Error(ILLEGAL_DECIMAL);
             }
      }
} /* end Check_Decimal */

/* ************************************************************************** */

/* Look for label definitions in the original symbol table                    */

struct sym_bkt *Original_Label_Lookup(s)
register char *s; 
{ register struct sym_bkt *sbp;
  register int Save;
  register char *p;
  char local[50];

  if ((*s >= '0') && (*s <= '9'))
     { p = local;
       while (*p++ = *s++);
       p--;
       s = Last_symbol->name_s;
       while (*p++ = *s++);
       s = local; 
     }
  /* if symbol is here, return ptr to it, otherwise return null */
  for (sbp = sym_hash_tab[Save = Hash(s)]; sbp != Stab_NULL; sbp = sbp->next_s)
      if (strcmp(sbp->name_s, s) == 0) 
         return(sbp);
  return (Stab_NULL);
} /* end Original_Label_Lookup */

/* ************************************************************************* */

/* Look for labels in sdb symbol table.  This procedure is called only after */
/* search has been made in the original symbol table.  When this procedure   */
/* is called, it implies that the label has not been found in the original   */
/* symbol table, and that it should be a sdb symbol table.  Otherwise, it    */
/* is an error.                                                              */

struct stab_sym_bkt *Stab_Label_Lookup(s)
char *s;
{ struct stab_sym_bkt *p;

  p = stabkt_head;
  while (p != Stab_NULL)
        { if (strcmp(p->ch, s) == 0)
             return(p);
          p = p->next_stab;
        }
  return (Stab_NULL);
} /* end Stab_Label_Lookup */

/* ************************************************************************** */

/* This will go through the entire sdb symbol table before  the second pass   */
/* to look for all labels with unresolved address and try to resolve them.    */

Find_Label_Value()
{ struct stab_sym_bkt *t, *f;
  struct sym_bkt *sbp_lab;
  
  t = stabkt_head;

  while (t != Stab_NULL)            /* go through the entire sdb symbol table */
        { if (t->tag == STABFLOATING)            /* look for unresolved value */
             { sbp_lab = Original_Label_Lookup(t->label); 
               /* if not found in original symbol table */
               /* try the sdb symbol table              */
               if (sbp_lab == Stab_NULL) 
                  { f = Stab_Label_Lookup(t->label);
                    if (f == Stab_NULL)
                       printf("\n Label %s not found.", t->label);
                    else { t->value = f->value;
                           /* printf("\n Found label = %s in Stab Tab", t->label);
                            * printf("\n with  f->value = 0%o", f->value);
                            * printf("\n with  t->value = 0%o", t->value);
                            */
                         }
                  }
               else { t->value = sbp_lab->value_s;
                      /*  printf("\n Found label = %s in Original Tab", t->label);
                       *  printf("\n with  sbp_lab->value_s = 0%o", sbp_lab->value_s);
                       *  printf("\n with  t->value = 0%o", t->value); 
                       */
                    }
             }
           t = t->next_stab;
         } /* end while */
} /* end Find_Label_Value */

/* ************************************************************************** */
