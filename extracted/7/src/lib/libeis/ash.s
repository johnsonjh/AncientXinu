/
/ PDP-11 ARITHMETIC SHIFT ROUTINE
/
/ This routine will arithmetically shift a 16-bit number up to 31 bits left
/ or 32 bits right. It also sets condition codes properly upon exit in the
/ location psw.
/
/ DEFINITION:
/ Arithmetic Shift      DO                                      072RSS
/ Operation:    R <- R shifted arithmetically NN places to the right or left
/               where NN = (src)<5:0>
/ Condition     N: set if result < 0
/ Codes:        Z: set if result = 0
/               V: set if sign of register changed during shift
/               C: loaded from last bit shifted out of register. Cleared if
/                  NN = 0.
/ Description:  ... The contents of the register are shifted right or left
/               the number of times specified by the source operand. The
/               shift count is taken as the low-order 6 bits of the source
/               operand. This number ranges from -32 to +31. Negative is a
/               right shift and positive is a left shift.
/
/ (copied from MICROCOMPUTERS AND MEMORIES, c. 1981 by Digital Equipment
/ Corporation; p. 89)
/
/ ALGORITHM:
/   The following C version is meant to be a guide ONLY; the code below  is
/ a  hand-compilation  of  this  program  (essentially). (The given version
/ ignores C parameter passing conventions.)
/
/ ash(n, s, psw)
/ int n;                /* number to be shifted */
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
/       mov     r0,-(sp)                / push number to be shifted onto the stack
/       jsr     pc,shift2               / call the simulation routine
/       mov     (sp)+,r0                / save the result
/       tst     (sp)+                   / pop garbage word
/       mtps    (sp)+                   / put in the new psw
/
/ WARNING:
/ THIS ROUTINE DOES NOT FOLLOW THE CALLING CONVENTIONS OF C. DO NOT CALL THIS
/ USING THE USUAL C CONVENTIONS; USE THE CALLING SEQUENCE GIVEN ABOVE.
/
/ ASSUMPTIONS:
/ the stack must be set up correctly
/ the routine must be called via "jsr pc,shift2" since it uses "rts pc"
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
                /       position on stack after r0 - r4 are pushed
sinpsw=20               / psw (on entry)
sotpsw=20               / psw (on exit)
sshift=16               / number of bits to be shifted (on entry)
snumbr=14               / number to be shifted (on entry); result (on exit)
                / register allocation
rshift=r0               / number of bits remaining to be shifted
rnumbr=r1               / number being shifted
rsign=r2                / sum of sign bits of previous shifts
rpsw=r3                 / psw
rtemp=r4                / used to save number of bits to be shifted
/
/*** declare the routine name to be global so anyone can use it
.globl shift2
.text
shift2:
/*** push the registers onto the stack
        mov     r0,-(lr)
        mov     r1,-(lr)
        mov     r2,-(lr)
        mov     r3,-(lr)
        mov     r4,-(lr)                / note r5 is not used
/*** set up registers and initialize condition codes
        mov     sinpsw(lr),rpsw         / get psw
        bic     $17,rpsw                / clear its condition codes
        clr     rsign                   / no sign bits yet
        mov     snumbr(lr),rnumbr       / get number to be shifted
        mov     sshift(lr),rtemp        / get the shift count
        bic     $177700,rtemp
        beq     setsgn                  / if 0, done
        cmp     rtemp,$31               / treat as 2's complement 6-bit number
        ble     possrt                  / if <= 31, shift left that many bits
        mov     $100,rshift             / if > 31, treat as negative; subtract
        sub     rtemp,rshift            /   from 64 to get number of bits to be
        mov     rshift,rtemp            /     shifted right; save count
/*** negative (right) shift
neglop: asr     rnumbr                  / shift 1 bit right
                                        / note the sign will never change as the
                                        /   sign bit is replicated
        sob     rshift,neglop           / go back unless done
        br      setc                    / set C bit properly
/*** positive (left) shift
possrt: mov     rtemp,rshift            / save original shift count
        inc     rtemp                   / get final shift total
        tst     rnumbr
        bpl     poslop                  / add sign bit to rsign
        inc     rsign
poslop: asl     rnumbr                  / shift 1 bit left
        bpl     poslnd                  / add sign bit to rsign
        inc     rsign
poslnd: sob     rshift,poslop           / go back unless done
setc:   bcc     setv                    / last bit shifted a 1
        bis     $1,rpsw                 / if so, set C bit
/*** set V bit correctly
setv:   tst     rsign                   / now for the V bit; no sign change if
        beq     setsgn                  /   rnumbr was always positive (rsign
        cmp     rsign,rtemp             /     == 0) or always negative (rsign
        beq     setsgn                  /       == original shift count + 1); else,
        bis     $2,rpsw                 /         sign bit changed
/*** set the sign bits correctly
setsgn: tst     rnumbr                  / get the right condition codes
        blt     setn                    /   of the result
        bgt     out                     / adios, amigo!
        bis     $4,rpsw                 / if zero, set the Z bit
        br      out                     / au revoir, mon ami!
setn:   bis     $10,rpsw                / if negative, set the N bit
/*** restore the registers after saving results
out:    mov     rnumbr,snumbr(lr)       / save result
        mov     rpsw,sotpsw(lr)         / save psw
        mov     (lr)+,r4                / restore registers
        mov     (lr)+,r3
        mov     (lr)+,r2
        mov     (lr)+,r1
        mov     (lr)+,r0
        rts     pc                      / do svidanya, tovarishch!
.data
