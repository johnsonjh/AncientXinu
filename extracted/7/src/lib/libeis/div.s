/
/ PDP-11 DIVIDE ROUTINE
/
/ This routine will divide a 32-bit signed number by a 16-bit signed number using
/ two's complement arithmetic. It also sets the condition codes properly upon exit
/ in the location spsw.
/
/ DEFINITION:
/ Divide                DO                                      071RSS
/ Operation:    R,Rv1 <- R,Rv1 / (src)
/ Condition     N: set if quotient < 0 (unspecified if V = 1).
/ Codes:        Z: set if quotient = 0 (unspecified if V = 1).
/               V: set if source = 0 or if quotient cannot be represented as a
/                  16-bit 2's complement number. R,Rv1 are unpredictable if V
/                  is set.
/               C: set if divide by 0 attempted.
/ Description:  ... The 32-bit 2's complement integer in R and Rv1 is divided by
/               the source operand. The quotient is left in R; the remainder in
/               Rv1 is of the same sign as the dividend. R must be even.
/
/ (copied from MICROCOMPUTERS AND MEMORIES, c. 1981 by Digital Equipment
/ Corporation; pp. 103 - 104)
/
/ EXAMPLE:
/       4 DIV  3 =  1                            4 MOD  3 =  1
/      -4 DIV -3 =  1                           -4 MOD -3 = -1
/       4 DIV -3 = -1                            4 MOD -3 =  1
/      -4 DIV  3 = -1                           -4 MOD  3 = -1
/
/ ALGORITHM:
/   The algorithm is the conventional shift-and-subtract one. The following C
/ version is meant to be a guide ONLY; the code below is a hand-compilation of
/ this program (essentially). (The given version ignores C parameter passing
/ conventions.)
/
/ div(a, b, q, r, psw)
/ long a;                         /* dividend */
/ long b;                         /* divisor */
/ int q;                          /* quotient */
/ int r;                          /* remainder */
/ int psw;                        /* processor status word */
/ {
/         int aneg, bneg;         /* 1 if corresponding variable is < 0 */
/         int c;                  /* count of shifts of quotient */
/
/         q = c = aneg = bneg = 0;/* initialize everything */
/         psw &= ~017;
/                                 /* handle 0 divisor or dividend */
/         if (b == 0L){ psw |= 03; return; }
/         if (a == 0L){ psw |= 07; return; }
/                                 /* handle negative divisor, dividend */
/         if (b < 0L){ b = -b; bneg++; }
/         if (a < 0L){ a = -a; aneg++; }
/                                 /* compute initial divisor shift */
/         while (a > b && !(b & 020000000000L)){
/                 b <<= 1;
/                 c++;
/         }
/                                 /* division loop */
/         while(1){
/                 q <<= 1;                /* one more subtraction */
/                 if (q & 0100000) psw |= 02;             /* flag overflow */
/                 if (b <= a){            /* can subtract */
/                         q += 1;                         /* reset quotient */
/                         if (q & 0100000) psw |= 02;     /* flag overflow */
/                         if ((a -= b) == 0L) break;      /* exact division */
/                 }
/                 if (--c < 0) break;     /* see if all shifts done */
/                 b >>= 1;                /* nope - halve divisor */
/         }
/                                 /* set remainder sign properly */
/         if (aneg != 0) a = -a;
/         r = (int) a;
/                                 /* shift quotient as required */
/         while(--c >= 0){
/                 q <<= 1;                /* a shift left */
/                 if (q & 0100000) psw |= 02;     /* flag overflow */
/         }
/                                 /* set quotient sign */
/         if (aneg != bneg){ q = -q; psw |= 010; }
/ }
/
/ CALLING SEQUENCE:
/ code to simulate the instruction "div A,r0":
/       mfps    -(sp)                   / push psw onto stack
/       mov     A,-(sp)                 / push divisor onto stack
/       mov     r0,-(sp)                / push high word of dividend onto stack
/       mov     r1,-(sp)                / push low word of dividend onto stack
/       jsr     pc,over2                / call the simulation routine
/       mov     (sp)+,r1                / save the remainder
/       mov     (sp)+,r0                / save the quotient
/       tst     (sp)+                   / pop garbage word
/       mtps    (sp)+                   / put in the new psw
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
sinpsw=24               / psw (on entry)
sotpsw=24               / psw (on exit)
sblow=22                / low word of divisor (on entry); ignored (on exit)
sahigh=20               / high word of dividend (on entry)
squot=20                / quotient (on exit)
salow=16                / low word of dividend (on entry); remainder (on exit)
                / stack allocation ... temporaries (ie., local variables)
saneg=-2                / 1 if dividend negative
sbneg=-4                / 1 if divisor negative
                / register allocation
rahigh=r0               / high word of dividend
ralow=r1                / low word of dividend
rbhigh=r2               / high word of divisor
rblow=r3                / low word of divisor
rcount=r4               / number of times divisor shifted
rquot=r5                / quotient
/
/*** declare the routine name to be global so anyone can use it
.globl  over2
.text
over2:
/*** push the registers onto the stack
        mov     r0,-(lr)
        mov     r1,-(lr)
        mov     r2,-(lr)
        mov     r3,-(lr)
        mov     r4,-(lr)
        mov     r5,-(lr)
/*** set up registers and initialize condition codes
        clr     rquot                   / no quotient yet
        clr     saneg(lr)               / for now, they're positive
        clr     sbneg(lr)
        clr     rcount                  / no shifts yet
        bic     $17,sinpsw(lr)          / clear condition codes
        mov     sahigh(lr),rahigh       / load dividend and divisor into
        mov     salow(lr),ralow         /   the right registers (note the high
        clr     rbhigh                  /     word of the divisor is 0)
        mov     sblow(lr),rblow
/*** see if either divisor or dividend is 0
        bne     chkdnd                  / high word of divisor is 0
        bis     $3,sinpsw(lr)           / flag a divide by 0
        br      out                     / adios, amigo!
chkdnd: tst     rahigh                  / see if both high and low words of the
        bne     negdiv                  /   dividend are 0
        tst     ralow
        bne     negdiv
        bis     $7,sinpsw(lr)           / set the Z bit (quotient, remainder 0)
        br      out                     / au revoir, mon ami!
/*** now do the signs
negdiv: tst     rblow                   / again, we need only look at the low
        bge     negdnd                  /   word of the divisor
        neg     rblow                   / negate it
        inc     sbneg(lr)               / set flag that divisor is negative
negdnd: tst     rahigh                  / just check the high word of the divi-
        bge     inicnt                  /   dend (it's double precision)
        neg     rahigh                  / a double precision negation
        neg     ralow
        sbc     rahigh
        inc     saneg(lr)               / set flag that dividend is negative
/*** figure out the first subtraction
inicnt: cmp     rahigh,rbhigh           / see if dividend is bigger than divisor
        blt     divlop                  /   if so, double divisor until it is
        bgt     bittst                  /     as big as dividend; then run the
        cmp     ralow,rblow             /       division loop
        blos    divlop
bittst: bit     $100000,rbhigh          / see if MSB of divisor set; if so, can't
        bne     divlop                  /   shift, so try dividing
        asl     rblow                   / this doubles the divisor
        rol     rbhigh
        inc     rcount                  / keep track of how many times divisor
        br      inicnt                  /   was doubled
/*** this next loop does the actual division
divlop: asl     rquot                   / one more division made, so double
        bit     $100000,rquot           /   the quotient. if MSB overflow just
        beq     noov1                   /     occurred, so set C bit, and keep
        bis     $2,sinpsw(lr)           /       on truckin' ...
noov1:  cmp     rbhigh,rahigh           / if divisor is bigger than dividend
        blo     incquo                  /   here, what's left is the remainder
        bhi     deccnt                  /     since no more subtractions are
        cmp     rblow,ralow             /       possible
        bhi     deccnt
incquo: inc     rquot                   / the divisor can be subtracted from the
        bit     $100000,rquot           /   dividend; do it and check for over-
        beq     noov2                   /     flow (process if present)
        bis     $2,sinpsw(lr)
noov2:  sub     rblow,ralow             / this does a double precision
        sbc     rahigh                  /   subtraction
        sub     rbhigh,rahigh
        bne     deccnt                  / if the dividend is 0, done with the
        tst     ralow                   /   division loop
        beq     setsgn
deccnt: dec     rcount                  / divisor must be shifted as many times as
        blt     setsgn                  /   there were initial shifts; here,
        asr     rbhigh                  /     chalk up another such shift
        ror     rblow
        br      divlop
/*** this part sets the sign of the remainder as required
setsgn: tst     saneg(lr)               / make the remainder have the same
        beq     fixquo                  /   sign as the dividend (division def-
        neg     ralow                   /     inition, see description)
/*** this makes the quotient the right size
fixquo: dec     rcount                  / now shift the quotient left until all
        blt     quosgn                  /   initial shifts are covered
        asl     rquot
        bit     $100000,rquot           / if MSB is set, there is overflow;
        beq     fixquo                  /   handle it
        bis     $2,sinpsw(lr)
        br      fixquo
/*** set the sign of the quotient
quosgn: cmp     saneg(lr),sbneg(lr)     / if divisor and dividend have the same
        beq     out                     /   sign, the quotient is positive; else,
        neg     rquot                   /     it is negative (arithmetic
        bis     $10,sinpsw(lr)          /       definition of division)
/*** restore the registers after saving results
out:    mov     ralow,salow(lr)         / save remainder
        mov     rquot,squot(lr)         / save quotient
        mov     sinpsw(lr),sotpsw(lr)   / save psw
        mov     (lr)+,r5                / restore registers
        mov     (lr)+,r4
        mov     (lr)+,r3
        mov     (lr)+,r2
        mov     (lr)+,r1
        mov     (lr)+,r0
        rts     pc                      / do svidanya, tovarishch!
.data
