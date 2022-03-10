/
/ PDP-11 ARITHMETIC SHIFT COMBINED ROUTINE
/
/ This routine will arithmetically shift a 32-bit number up to 31 bits left
/ or 32 bits right. It also sets condition codes properly upon exit in the
/ location psw.
/
/ DEFINITION:
/ Arithmetic Shift      DO                                      073RSS
/     Combined
/ Operation:    tmp <- R,Rv1
/               tmp <- tmp shifted NN bits
/               R <- tmp<31:16>
/               Rv1 <- tmp<15:0>
/               The double word R,Rv1 is shifted NN places to the right
/               or left, where NN= (src)<5:0>
/ Condition     N: set if result < 0
/ Codes:        Z: set if result = 0
/               V: set if sign bit changes during the shift
/               C: loaded with high-order bit when left shift;
/                  loaded with low-order bit when right shift
/                  (loaded with the last bit shifted out of the 32-bit
/                  operand)
/ Description:  ... The contents of the specified register R, and the register
/               Rv1 are treated as a single 32-bit operand and are shifted by
/               the number of bits specified by the count field (bits <5:0> of
/               the source operand) and the registers are replaced by the re-
/               sult. First, bits <31:16> of the result are stored in register
/               R. Then, bits <15:0> of the result are stored in register Rv1.
/               A negative count signifies a right shift. A positive count
/               signifies a left shift. A zero count implies no shift, but
/               condition codes are affected. Condition codes are always set
/               on the 32-bit result.
/ Note:         1) The sign bit of register R is replicated in shifts to the
/               right. The least significant bit is filled with zero in shifts
/               to the left. The C bit stores the last bit shifted out. 2) In-
/               teger overflow occurs on a left shift if any bit shifted into
/               the sign position differs from the initial sign of the regis-
/               ter.
/
/ (copied from MICROCOMPUTERS AND MEMORIES, c. 1981 by Digital Equipment
/ Corporation; pp. 89 - 90)
/
/ ALGORITHM:
/   The following C version is meant to be a guide ONLY; the code below  is
/ a  hand-compilation  of  this  program  (essentially). (The given version
/ ignores C parameter passing conventions.)
/
/ ashc(n, s, psw)
/ long n;               /* number to be shifted */
/ int s;                /* number of shifts */
/ int psw;              /* processor status word */
/ {
/       int origs;      /* original shift count */
/       int scnt;       /* sign bit count */
/
/       psw &= ~017;    /* initialize */
/       s &= ~077;
/       scnt = 0;
/                       /* do the shifting */
/       if (s == 0)     /* no shift */
/               psw |= 01;
/       else if (s > 31){    /* negative shift */
/               origs = s = 64 - s;
/               while(s-- > 0){
/                       n >>= 1;        /* remember, we do have this */
/                       scnt += (n < 0);
/               }
/               "if C bit set, psw |= 01";
/       }
/       else{           /* positive shift */
/               origs = s;
/               while(s-- > 0){
/                       n <<= 1;        /* remember, we do have this */
/                       scnt += (n < 0);
/               }
/               "if C bit set, psw |= 01";
/       }
/                       /* set V bit */
/       if (scnt != 0 && scnt != origs)
/               psw |= 02;
/                       /* set sign */
/       if (n < 0) psw |= 010;
/       else if (n == 0) psw |= 04;
/ }
/
/ CALLING SEQUENCE:
/ code to simulate the instruction "ash A,r0":
/       mfps    -(sp)                   / get psw
/       mov     A,-(sp)                 / push shift count onto the stack
/       mov     r0,-(sp)                / push high word onto the stack
/       mov     r1,-(sp)                / push low word onto the stack
/       jsr     pc,cshft2               / call the simulation routine
/       mov     (sp)+,r1                / save the low word
/       mov     (sp)+,r0                / save the high word
/       tst     (sp)+                   / pop garbage word
/       mtps    (sp)+                   / put in the new psw
/
/ WARNING:
/ THIS ROUTINE DOES NOT FOLLOW THE CALLING CONVENTIONS OF C. DO NOT CALL THIS
/ USING THE USUAL C CONVENTIONS; USE THE CALLING SEQUENCE GIVEN ABOVE.
/
/ ASSUMPTIONS:
/ the stack must be set up correctly
/ the routine must be called via "jsr pc,cshft2" since it uses "rts pc"
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
sshift=22               / number of bits to be shifted (on entry)
snumbr=20               / high word of number to be shifted (on entry); result (on exit)
snumb2=16               / low word of number to be shifted (on entry); result (on exit)
                / register allocation
rshift=r0               / number of bits remaining to be shifted
rnumbr=r1               / high word of number being shifted
rnumb2=r2               / low word of number being shifted
rsign=r3                / sum of sign bits of previous shifts
rpsw=r4                 / psw
rtemp=r5                / used to save number of bits to be shifted
/
/*** declare the routine name to be global so anyone can use it
.globl cshft2
.text
cshft2:
/*** push the registers onto the stack
        mov     r0,-(lr)
        mov     r1,-(lr)
        mov     r2,-(lr)
        mov     r3,-(lr)
        mov     r4,-(lr)
        mov     r5,-(lr)
/*** set up registers and initialize condition codes
        mov     sinpsw(lr),rpsw         / get psw
        bic     $17,rpsw                / clear its condition codes
        clr     rsign                   / no sign bits yet
        mov     snumbr(lr),rnumbr       / get number to be shifted
        mov     snumb2(lr),rnumb2
        mov     sshift(lr),rtemp        / get the shift count
        bic     $177700,rtemp
        beq     setsgn                  / if 0, done
        cmp     rtemp,$31.              / treat as 2's complement 6-bit number
        ble     possrt                  / if <= 31, shift left that many bits
        mov     $64.,rshift             / if > 31, treat as negative; subtract
        sub     rtemp,rshift            /   from 64 to get number of bits to be
        mov     rshift,rtemp            /     shifted right; save count
/*** negative (right) shift
neglop: asr     rnumbr                  / shift 1 bit right; note sign will never
        ror     rnumb2                  /   change as the sign bit is replicated
        sob     rshift,neglop           / go back unless done
        br      setc                    / set C bit properly
/*** positive (left) shift
possrt: mov     rtemp,rshift            / save original shift count
poslop: asl     rnumb2                  / shift 1 bit left
        rol     rnumbr
        bpl     poslnd                  / add sign bit to rsign
        inc     rsign
poslnd: sob     rshift,poslop           / go back unless done
setc:   bcc     setv                    / last bit shifted a 1
        bis     $1,rpsw                 / if so, set C bit
/*** set V bit correctly
setv:   tst     rsign                   / now for the V bit; no sign change if
        beq     setsgn                  /   rnumbr was always positive (rsign
        cmp     rsign,rtemp             /     == 0) or always negative (rsign
        beq     setsgn                  /       == original shift count); else,
        bis     $2,rpsw                 /         sign bit changed
/*** set the sign bits correctly
setsgn: tst     rnumbr                  / get the right condition codes
        blt     setn                    /   of the result
        bgt     out                     / adios, amigo!
        tst     rnumb2                  / test low word if high word is 0
        bne     out                     / cannot be negative
        bis     $4,rpsw                 / if zero, set the Z bit
        br      out                     / au revoir, mon ami!
setn:   bis     $10,rpsw                / if negative, set the N bit
/*** restore the registers after saving results
out:    mov     rnumbr,snumbr(lr)       / save result
        mov     rnumb2,snumb2(lr)
        mov     rpsw,sotpsw(lr)         / save psw
        mov     (lr)+,r5                / restore registers
        mov     (lr)+,r4
        mov     (lr)+,r3
        mov     (lr)+,r2
        mov     (lr)+,r1
        mov     (lr)+,r0
        rts     pc                      / do svidanya, tovarishch!
.data
