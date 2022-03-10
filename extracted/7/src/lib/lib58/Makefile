#
#   Make file for tu58 library routines
#

DIR	= /usr/Xinu
LIBDIR	= $(DIR)/lib
INCDIR	= $(DIR)/include
RANLIB	= $(DIR)/bin/ranlib11
CFLAGS	= -O -c -I$(INCDIR)

all:		lib58.a

install:	all
		cp lib58.a $(LIBDIR)
		chmod 644 $(LIBDIR)/lib58.a
		$(RANLIB) $(LIBDIR)/lib58.a

clean:
		rm -f *.o *.a

lib58.a:	tperror.o tu58.o
		/bin/rm -rf lib58.a
		ar cr lib58.a tperror.o	tu58.o

tperror.o:	tperror.c
		cc $(CFLAGS) tperror.c

tu58.o:		tu58.c
		cc $(CFLAGS) tu58.c

tperror.c:	/usr/Xinu/include/tu58errno.h

tu58.c:		/usr/Xinu/include/tu58.h		\
		/usr/Xinu/include/tu58io.h		\
		/usr/Xinu/include/tu58errno.h

