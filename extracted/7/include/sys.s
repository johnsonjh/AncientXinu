/ added at purdue while we hack up the assembler
/ matt bishop, ken dickey  (mab & kad)

mfps    = 0106700 ^ tst
mtps    = 0106400 ^ tst
rtt     = 0000006
rti     = 0000002
halt    = 0000000
reset   = 0000005
nop     = 0000240
wait	= 0000001

/ Added for the LSI11/23 by Steven Munson (sbm)

trap	= sys
mfpd	= 106500 ^ tst
mfpi	= 006500 ^ tst
mtpd	= 106600 ^ tst
mtpi	= 006600 ^ tst
