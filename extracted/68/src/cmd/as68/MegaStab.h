#define Stab_NULL        0
#define MISSING_STRING   1
#define MISSING_QUOTE    2
#define ILLEGAL_HEX      3
#define ILLEGAL_OCTAL    4
#define ILLEGAL_DECIMAL  5

char *Stab_Messages[] = {
     /* 1 */    "Expecting a string in .stabs directive.", 
     /* 2 */    "At least 1 quote mark is missing from string.",
     /* 3 */    "Illegal hexadecimal digit found.",
     /* 4 */    "Illegal octal digit found.",
     /* 5 */    "Illegal decimal digit found.",
                0                   /* YIP */
};

