#!/bin/sh
: 'en - defines eload entry point in .entry'
:
ENTRY=.entry
/usr/Xinu/bin/nm11 eload | /bin/awk '				\
	/estart/ {						\
		print "#define ETH_INITPC " $1			\
	}							\
    ' > ,$ENTRY
/bin/mv ,$ENTRY $ENTRY
