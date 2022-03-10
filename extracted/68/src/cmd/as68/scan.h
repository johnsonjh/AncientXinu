
/* stab symbol table bucket - YIP */
struct stab_sym_bkt {
         char                *ch;        /* string name   */
         unsigned char       type;       /* field "type"  */
         char                other;      /* field "other" */
         short               desc;       /* field "desc"  */
         unsigned long       value;      /* field "value" */
         short               id;         /* 1 = string, 0 = no string */
         short               tag;        /* STABFLOATING/STABFIX */
         char                *label;     /* actual string of the label */
         struct stab_sym_bkt *next_stab;
};

                                    /* YIP */
struct stab_sym_bkt *stabkt_head,   /* head of stab symbol bucket linked list */
                    *stabkt_tail;   /* tail of stab symbol bkcket linked list */

