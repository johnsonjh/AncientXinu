#!/bin/sh
: 'av - automatic version generator increments .version in current directory'
:
: 'Note: version 255 is reserved for ROM version (always accepted)'
:
VERSFILE=.version
if test  ! -f $VERSFILE -o ! -s $VERSFILE
then echo "#define VERSION -1" > $VERSFILE
fi
awk '								\
	/VERSION/ {						\
		print "#define VERSION " ($3 +1) % 255;	\
		print "#define ROMVERS 255"			\
	}							\
    ' < $VERSFILE > ,$VERSFILE
/bin/mv ,$VERSFILE $VERSFILE

