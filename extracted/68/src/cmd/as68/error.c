#include "mical.h"

/*
 *
 *  Sys_Error(), Prog_Error(), and Describe_Error() for mical assembler 
 *
 */


char *E_messages[] = {
/* 0 */		"<unused>",
/* 1 */		"Missing .end statement",
/* 2 */		"Invalid character",
/* 3 */		"Multiply defined symbol",
/* 4 */		"Symbol storage exceeded",
/* 5 */		"Offset too large",
/* 6 */		"Symbol too long",
/* 7 */		"Undefined symbol",
/* 8 */		"Invalid constant",
/* 9 */		"Invalid term",
/* 10 */	"Invalid operator",
/* 11 */	"Non-relocatable expression",
/* 12 */	"Wrong type for instruction",
/* 13 */	"Invalid operand",
/* 14 */	"Invalid symbol",
/* 15 */	"Invalid assignment",
/* 16 */	"Too many labels",
/* 17 */	"Invalid op-code",
/* 18 */	"Invalid entry point",
/* 19 */	"Invalid string",
/* 20 */	"Bad filename or too many levels",
/* 21 */	"Warning--attribute ignored",
/* 22 */	".Error statement",
/* 23 */	"Too many levels: statement ignored",
/* 24 */	"Invalid condition",
/* 25 */	"Wrong number of operands",
/* 26 */	"Line too long",
/* 27 */	"Invalid register expression",
/* 28 */	"Invalid machine address",
/* 29 */	"Unimplemented directive",
/* 30 */	"Cannot open inserted file",
/* 31 */	"Invalid string",
/* 32 */	"Too many macro arguments",
/* 33 */	"Invalid macro argument",
/* 34 */	"Invalid formal argument",
/* 35 */	"Inappropriate .endc statement; ignored",
/* 36 */	"Warning--relative address may be out of range",
/* 37 */	"Warning--invalid argument; ignored",
/* 38 */	"Invalid instruction vector index",
/* 39 */	"Invalid instruction vector",
/* 40 */	"Invalid macro name",
/* 41 */	"Unable to expand time macro",
/* 42 */	"Bad csect",
/* 43 */	"Odd address",
		0
} ;

int Errors = 0;		/* Number of errors in this pass */

/* Sys_Error is called when a System Error occurs, that is, something is wrong
   with the assembler itself. Each routine of the assembler usually checks the
   arguments passed to it for validity, and calls this routine if its
   parameters are invalid. Explanation is a string suitable for a printf
   control string which explains the error, and Number is the value of the
   offending parameter.  This routine will not return.
*/
Sys_Error(Explanation,Number)
char *Explanation;
{
	fprintf(stderr, "Assembler Error-- ");
	fprintf(stderr, Explanation,Number);
	abort();
}

/* This is called whenever the assembler recognizes an error in the current
statement. It registers the error, so that an error code will be listed with
the statement, and a description of the error will be printed at the end of
the listing */

Prog_Error(code)
register int code;
{
	register int i;

	if (Pass != 2) return;		/* no errors on pass 1 */
	Errors++;				/* increment error count */
	fprintf(stderr,"error (%d): %s\n",Line_no,E_messages[code]);
}

/* Prog_Warning registers a warning on a statement. A warning is like an error,
	in that something is probably amiss, but the assembler will still try 
	to generate the .rel file. 
*/

Prog_Warning(code){

	if (Pass != 2) return;
	fprintf(stderr,"warning (%d): %s\n",Line_no,E_messages[code]);
}
