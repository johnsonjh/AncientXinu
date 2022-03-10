#
# make the C cross compiler
#

DESTDIR = /usr/Xinu/bin
LIBDIR  = /usr/Xinu/lib
CFLAGS  = -O
LIBES   =

all:		c0 c1 c2 cc11

clean:
		-rm -fr c0 c1 c2 *.o core cvopt table.i

install:	all
		cp c0 $(LIBDIR)/c0.11
		cp c1 $(LIBDIR)/c1.11
		cp c2 $(LIBDIR)/c2.11
		strip $(LIBDIR)/c?.11
		chmod 711 $(LIBDIR)/c?.11
		cp cc11 $(DESTDIR)/cc11

cc11:		cc11.c
		cc -o cc11 cc11.c

fcp:		fc1
		cp fc1 $(DESTDIR)/lib/fc1
		rm fc1 c1*.o

c0:		c00.o c01.o c02.o c03.o c04.o c05.o
		cc $(CFLAGS) -n -o c0 c00.o c01.o c02.o c03.o c04.o c05.o \
			 $(LIBES)

c00.o c01.o c02.o c03.o c04.o c05.o: c0.h

c1:		c10.o c11.o c12.o c13.o table.o
		cc $(CFLAGS) -n -o c1 c10.o c11.o c12.o c13.o table.o \
			 $(LIBES)

fc1:		c10.o c11.o c12.o c13.o table.o
		cc $(CFLAGS) -f -o fc1 c10.o c11.o c12.o c13.o table.o \
			 $(LIBES)

c10.o c11.o c12.o c13.o: c1.h

table.o:	table.i
		as -o table.o table.i

table.i:	table.s cvopt sedscript
		./cvopt <table.s | sed -f sedscript >table.i

c2:		c20.o c21.o
		cc $(CFLAGS) -i -o c2 c20.o c21.o $(LIBES)

c20.o c21.o:	c2.h

cvopt:		cvopt.c
		cc $(CFLAGS) -o cvopt cvopt.c $(LIBES)
