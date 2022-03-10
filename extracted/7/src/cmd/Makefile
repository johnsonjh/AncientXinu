#
#  Make cross-commands that run on the VAX
#

CFLAGS  = -O -s
DIR = /usr/Xinu
DESTDIR = $(DIR)/bin
LIBDIR  = $(DIR)/lib

FILES=		cprint		\
		cvt		\
		dd58		\
		ld11		\
		lorder11	\
		nm11		\
		ranlib11	\
		size11		\
		strip11

CFILES=		dd58.c		\
		ld11.c		\
		nm11.c		\
		ranlib11.c	\
		size11.c	\
		strip11.c

OFILES=		dd58.o		\
		ld11.o		\
		nm11.o		\
		ranlib11.o	\
		size11.o	\
		strip11.o


all:		$(FILES)

install:	$(FILES)
		chmod 755 $(FILES)
		cp $(FILES) $(DESTDIR)

clean:
		-rm -f *.o core	$(FILES)

cprint:		cprint.sh
		sed 's/^[ 	]*//;/^:/d' <cprint.sh >cprint

cvt:		cvt.sh
		sed 's/^[ 	]*//;/^:/d' <cvt.sh >cvt

dd58:		dd58.c
		ranlib $(LIBDIR)/lib58.a
		$(CC) $(CFLAGS) -o dd58 dd58.c $(LIBDIR)/lib58.a

ld11:		ld11.c
		$(CC) $(CFLAGS) -o ld11 ld11.c

lorder11:	lorder11.sh
		sed 's/^[ 	]*//;/^:/d' <lorder11.sh >lorder11

nm11:		nm11.c
		$(CC) $(CFLAGS) -o nm11 nm11.c

ranlib11:	ranlib11.c
		$(CC) $(CFLAGS) -o ranlib11 ranlib11.c

size11:		size11.c
		$(CC) $(CFLAGS) -o size11 size11.c

strip11:	strip11.c
		$(CC) $(CFLAGS) -o strip11 strip11.c

depend:
		grep '^#include' $(CFILES) \
		| sed -e '/<.*>/d' -e 's/:[^"]*"\([^"]*\)".*/: \1/' \
		| sed 's/\.c/.o/' >Makedep
		echo '/^# DO NOT DELETE	THIS LINE/+2,$$d' >eddep
		echo '$$r Makedep' >>eddep
		echo 'w' >>eddep
		cp Makefile ,Makefile
		ed - ,Makefile < eddep
		rm -rf  eddep Makedep Makefile.old
		echo '#	DEPENDENCIES MUST END AT END OF	FILE' >> ,Makefile
		echo '#	IF YOU PUT STUFF HERE IT WILL GO AWAY' >> ,Makefile
		echo '#	see make depend	above' >> ,Makefile
		cp Makefile Makefile.old
		mv ,Makefile Makefile

# DO NOT DELETE	THIS LINE
# DEPENDENCIES MUST END	AT END OF FILE
dd58.o: /usr/Xinu/include/tu58io.h
dd58.o: /usr/Xinu/include/tu58errno.h
ld11.o: /usr/Xinu/include/pdp11/pagsiz.h
ld11.o: /usr/Xinu/include/pdp11/a.out.h
nm11.o: /usr/Xinu/include/a.out.h
ranlib11.o: /usr/Xinu/include/a.out.h
size11.o: /usr/Xinu/include/a.out.h
strip11.o: /usr/Xinu/include/a.out.h
#	DEPENDENCIES MUST END AT END OF	FILE
#	IF YOU PUT STUFF HERE IT WILL GO AWAY
#	see make depend	above
