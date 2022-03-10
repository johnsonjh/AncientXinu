/*
 *      DIVISION CONVERSION ROUTINE
 *
 * TO COMPILE RUN cc -O subDIV.c
 *
 *   Given an LSI-11 division instruction
 *      div a,b
 * this program will generate assembly code that will produce the equivalent
 * result. Condition codes are properly set.
 *   The replacement is done in two phases. The first runs through the
 * assembly code, replacing each "l:div A,r0" line (for example) with:
 * l:   mfps    -(sp)                   / get psw
 *      mov     A,-(sp)                 / push divisor onto the stack
 *      mov     r0,-(sp)                / push high word of dividend onto the stack
 *      mov     r1,-(sp)                / push low word of dividend onto the stack
 *      jsr     pc,over2                / call the simulation routine
 *      mov     (sp)+,r1                / save the remainder
 *      mov     (sp)+,r0                / save the quotient
 *      tst     (sp)+                   / restore the stack
 *      mtps    (sp)+                   / put in the new psw
 * (Recall that the destination register nust be even.) In no case are comments
 * provided. This follows the UNIX philosophy of incomprehensibility above all
 * else in assembly language.)
 *
 * Program by Matt Bishop, Purdue University (October 28, 1981)
 * for Doug Comer's 690d microcomputer seminar
 *
 */

/*
 * usual include crud
 */
#include <stdio.h>
#include <ctype.h>

#define LEN 30          /* max length of label, opcode, or operand */
#define DIV "div"       /* instruction to be replaced */

char label[LEN];        /* label of current assembly code line */
char operator[LEN];     /* operator of current assembly code line */
char foperand[LEN];     /* first operand of current assembly code line */
char soperand[LEN];     /* second operand of current assembly code line */
int rn;                 /* number of destination register */
int flag;               /* 1 if any substitutions were made, 0 if not */
int linect;             /* current line number */
char *pgnm;             /* name of program */

main(argc, argv)
int argc;
char **argv;
{
        char line[BUFSIZ];      /* input line */

        /* Initialize everything */
        pgnm = argv[0];
        flag = 0;
        linect = 0;

        /* phase 1 - loop through the file, line by line */
        while (fgets(line, BUFSIZ, stdin) != NULL){
                /* new line number */
                linect++;

                /* is line is a "div" instruction */
                if (finddiv(line)){

                        /* yes - be sure dest is a legal register */
                        if ((rn = isregis(soperand)) < 0 || rn > 6 || (rn&01)){
                                fprintf(stderr, "%s, line %d: second operand ", pgnm, linect);
                                fprintf(stderr, "of div must be an even numbered");
                                fprintf(stderr, " register\n");
                                printf("%s", line);
                                continue;
                        }

                        /* phase 1 substitution - see header comments */
                        flag = 1;
                        if (*label)
                                printf("%s:\t", label);
                        else
                                printf("\t");
                        printf("mfps\t-(sp)\n");
                        printf("\tmov\t%s,-(sp)\n", foperand);
                        printf("\tmov\t%s,-(sp)\n", soperand);
                        printf("\tmov\tr%d,-(sp)\n", rn+1);
                        printf("\tjsr\tpc,over2\n");
                        printf("\tmov\t(sp)+,r%d\n", rn+1);
                        printf("\tmov\t(sp)+,%s\n", soperand);
                        printf("\ttst\t(sp)+\n");
                        printf("\tmtps\t(sp)+\n");
                }
                else    /* not a "div" instruction - print whatever's there */
                        printf("%s", line);
        }

}

/*
 * returns -1 if its arg is not a register, number if it is
 */
int isregis(s)
char *s;
{
        if (s[0] != 'r' || s[2] != '\0')
                return(-1);
        return(s[1] - '0');
}

/*
 * This parses the input line
 */
int finddiv(s)
char *s;
{
        /* get label and operator (if any) */
        getlabel(&s, label);
        getoper(&s, operator);

        /* if not DIV, don't go on */
        if (strcmp(operator, DIV) != 0)
                return(0);

        /* it's a DIV -- get the operands */
        getand(&s, foperand);
        getand(&s, soperand);

        /* be sure they're really there */
        if (*foperand == '\0' || *soperand == '\0'){
                fprintf(stderr, "%s, line %d: div takes 2 operands\n", pgnm, linect);
                return(0);
        }

        /* success */
        return(1);
}

/*
 * This gets a label (if any) from the input line
 * Legal label chars are alphanumerics and _.~
 */
getlabel(s, l)
char **s, *l;
{
        char *p, *lp;

        /* grab first identifier on line */
        for(p = *s, lp = l; isalnum(*p) || *p == '_' || *p == '.' || *p == '~'; p++, lp++)
                *lp = *p;
        /* it's a label */
        if (*p == ':'){
                *lp = '\0';
                for(p++; isspace(*p); p++);  /* eat trailing spaces */
                *s = p;
        }
        else    /* bullfeathers */
                *l = '\0';
        return;
}

/*
 * This gets an operator (if any) from the input line
 * Legal operator chars are alphanumerics
 */
getoper(s, l)
char **s, *l;
{
        char *p;

        /* grap next identifier */
        for(p = *s; isalnum(*p); p++, l++)
                *l = *p;

        /* eat trailing spaces */
        for(; isspace(*p); p++);

        *l = '\0';
        *s = p;
        return;
}

/*
 * This gets an operand (if any) from the input line
 * Legal operand chars are alphanumerics and $()+-_.
 */
getand(s, l)
char **s, *l;
{
        char *p;

        /* grab the identifier */
        for(p = *s; isalnum(*p) || *p == '$' || *p == '(' || *p == ')' || *p == '-' || *p == '+' || *p == '.' || *p == '_'; p++, l++)
                *l = *p;

        /* eat trailing comma, spaces */
        if (*p == ',')
                p++;
        for(; isspace(*p); p++);

        *l = '\0';
        *s = p;
        return;
}
