/
/ PDP-11 MULTIPLY ROUTINE
/
/ This routine will multiply two 16-bit signed numbers using two's complement
/ arithmetic. It also sets the condition codes properly upon exit in the lo-
/ cation psw.
/
/ DEFINITION:
/ Multiply              DO                                      070RSS
/ Operation:    R,Rv1 <- R x (src)
/ Condition     N: set if product < 0
/ Codes:        Z: set if product = 0
/               V: cleared
/               C: set if the result is less than -2^15 or greater than
/                  or equal to 2^15 - 1
/ Description:  The contents of the destination register and source taken
/               as 2's complement integers are multiplied and stored in
/               the destination register and the succeeding register if R
/               is even. If R is odd, only the low orede product is stored.
/               ...
/
/ (copied from MICROCOMPUTERS AND MEMORIES, c. 1981 by Digital Equipment
/ Corporation; pp. 111 - 112)
/
/ ALGORITHM:
/   The algorithm is the conventional add-and-shift one. The following C
/ version is meant to be a guide ONLY; the code below is a hand-compilation of
/ this program (essentially). (The given version ignores C parameter passing
/ conventions.)
/
/ mul(a, b, prod, psw)
/ int a;                /* multiplier */
/ long b;               /* multiplicand */
/ long prod;            /* product */
/ int psw;              /* processor status word */
/ {
/       prod = 0L;      /* initialize */
/       psw &= ~017;
/                       /* handle a <= 0 */
/       if (a < 0){ a = -a; psw |= 010; }
/       else if (a == 0){ psw &= ~017; psw |= 04; return; }
/                       /* handle b <= 0 */
/       if (b < 0L){ b = -b; psw ^= 010; }
/       else if (b == 0L){ psw &= ~017; psw |= 04; return; }
/                       /* do the multiply */
/       while(a > 0){
/               if (a&01) prod += b;    /* LSB of a is 1 so add in b */
/               a >>= 1;                /* halve a */
/               b <<= 1;                /* double b */
/       }
/                       /* check for overflow */
/       if (psw & 010){
/               if (prod > 0177777L) psw |= 01; /* overflow */
/               prod = -prod;
/       }
/       else if (prod >= 077777L) psw |= 01;    /* overflow */
/ }
/
/ CALLING SEQUENCE:
/ code to simulate the instruction "mul A,r2":
/       mfps    (sp)                    / get psw
/       mov     A,-(sp)                 / push multiplier onto the stack
/       mov     r2,-(sp)                / push multiplicand onto the stack
/       jsr     pc,times2               / call the simulation routine
/       mov     (sp)+,r2                / save the low word of the product
/       mov     (sp)+,r3                / save the high word of the product
/       mtps    (sp)                    / put in the new psw
/
/ code to simulate the instruction "mul A,r1":
/       mfps    (sp)                    / get psw
/       mov     A,-(sp)                 / push multiplier onto the stack
/       mov     r1,-(sp)                / push multiplicand onto the stack
/       jsr     pc,times2               / call the simulation routine
/       tst     (sp)+                   / throw away the high word of the product
/       mov     (sp)+,r1                / save the low word of the product
/       mtps    (sp)                    / put in the new psw
/
/ WARNING:
/ THIS ROUTINE DOES NOT FOLLOW THE CALLING CONVENTIONS OF C. DO NOT CALL THIS
/ USING THE USUAL C CONVENTIONS; USE THE CALLING SEQUENCE GIVEN ABOVE.
/
/ ASSUMPTIONS:
/ the stack must be set up correctly
/ the routine must be called via "jsr pc,over2" since it uses "rts pc"
/       to return
/
/ AUTHOR:
/ Matt Bishop
/ Department of Computer Sciences
/ Purdue University
/ West Lafayette, IN  47907
/
/ Version 1, October 30, 1981
/
/
/ CONSTANTS
lr=sp           / linkage register
                / stack allocation ... parameter variables
                /       position on stack after r0 - r5 are pushed
spsw=22                 / psw (on entry and exit)
sblow=20                / multiplicand (on entry)
splow=20                / low word of product (on exit)
sa=16                   / multiplier (on entry)
sphigh=16               / high word of product (on exit)
                / register allocation
rphigh=r0               / high word of product
rplow=r1                / low word of product
rbhigh=r2               / high word of multiplicand
rblow=r3                / low word of multiplicand
ra=r4                   / multiplier
rpsw=r5                 / psw
/
/*** declare the routine name to be global so anyone can use it
.globl  times2
.text
times2:
/*** push the registers onto the stack
        mov     r0,-(sp)
        mov     r1,-(sp)
        mov     r2,-(sp)
        mov     r3,-(sp)
        mov     r4,-(sp)
        mov     r5,-(sp)
/*** set up registers and initialize condition codes
        clr     rphigh                  / clear product
        clr     rplow
        clr     rbhigh                  / clear high word of multiplier
        mov     sblow(lr),rblow         / get low word of multiplier
        mov     spsw(lr),rpsw           / get psw, clobber present condition
        bic     $17,rpsw                /   codes
        mov     sa(lr),ra               / get multiplier
/*** handle cases where multiplier is negative or 0
        bgt     apos
        beq     zero
        bis     $10,rpsw                / set N in psw
        neg     ra                      / make it positive
/*** handle cases where multiplicand is negative or 0
apos:   tst     rblow                   / need only test low word (high one
        bgt     muloop                  /   is 0 here)
        beq     zero
        mov     $10,rbhigh              / exclusive-or a negative condition
        xor     rbhigh,rpsw             /   code with the current one; this
        clr     rbhigh                  /     sets it correctly
        neg     rblow                   / make multiplicand positive
/*** do the multiply
muloop: asr     ra                      / shift multiplier
        bcc     nxtbit                  / test bit shifted out; if 1, add
        add     rblow,rplow             /   multiplicand to product (double
        adc     rphigh                  /     precision add)
        add     rbhigh,rphigh
nxtbit: asl     rblow                   / shift multiplicand (double pre-
        rol     rbhigh                  /   cision shift)
        tst     ra                      / continue this until multiplier
        bne     muloop                  /   is 0
/*** set the sign of the product
        bit     $10,rpsw                / if positive, check for overflow
        beq     ovflo
/*** check for overflow from a negative number
        tst     rphigh                  / if high word 0, negative product
        beq     mpneg                   /   did not overflow
        bis     $1,rpsw
mpneg:  neg     rplow                   / negate the product (to conform to
        neg     rphigh                  /   the condition code; use a double
        sbc     rphigh                  /     precision negation)
        br      out                     / adios, amigo!
/*** handle a zero factor
zero:   bic     $17,rpsw                / clear all condition codes
        bis     $4,rpsw                 / set the Z bit
        br      out                     / au revoir, mon ami!
/*** check for overflow from a positive number
ovflo:  tst     rphigh                  / test positive product for overflow
        bne     setcc                   /   (note you have to test the high
        cmp     $77777,rplow            /     word and see if the low word is
        bhi     out                     /       <= 2^15 - 1 (sign bit)
setcc:  bis     $1,rpsw                 / on overflow, set the C bit
/*** restore the registers after saving results
out:    mov     rpsw,spsw(lr)           / save psw
        mov     rplow,splow(lr)         / save product
        mov     rphigh,sphigh(lr)
        mov     (lr)+,r5                / restore registers
        mov     (lr)+,r4
        mov     (lr)+,r3
        mov     (lr)+,r2
        mov     (lr)+,r1
        mov     (lr)+,r0
        rts     pc                      / do svidanya, tovarishch!
.data
