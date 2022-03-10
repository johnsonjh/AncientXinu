#
#  Makefile for resident piece of Ethernet downloader
#

DIR	=	/usr/Xinu
LIB	=	$(DIR)/lib
LD	=	$(DIR)/bin/ld11
CC	=	$(DIR)/bin/cc11

.s.o:
		$(CC) -c -O $<

.c.o:
		$(CC) -c -O $<

all:		eload

install:	eload
		rcp eload gwen:eload
		rsh gwen download -cLSI -l1 -s eload

clean:
		rm -rf core eload *.o

eload:		startup.o eload.o eread.o ewrite.o erstart.o		\
		ewstart.o blkcopy.o blkequ.o netutil.o
		$(LD) -e estart -o eload				\
			startup.o eload.o eread.o ewrite.o		\
			erstart.o ewstart.o blkcopy.o blkequ.o		\
			netutil.o libsa.a $(LIB)/libxc.a

g:		eload
		rcp eload gwen:eload

down:		g
		rsh gwen download -cLSI -l1 -s eload

