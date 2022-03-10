#
#  Makefile for resident piece of PDP-11 Ethernet downloader
#

DIR	=	/usr/Xinu
DEST	=	$(DIR)/lib
LIB	=	$(DIR)/lib
BIN	=	$(DIR)/bin
CC	=	/bin/cc
LD	=	/bin/ld
CPP	=	/lib/cpp
CFLAGS	=	-c -O -I../h
SFLAGS	=	-I../h

.s.o:
		$(CPP) $(SFLAGS) $<	| as -o $*.o

.c.o:
		$(CC) $(CFLAGS) $<

all:		eload

install:	eload
		cp eload $(LIB)/eload.vax

eload.o:	.version eload.c
		$(CC) -c -O eload.c
	
eload:		startup.o eload.o eread.o ewrite.o erstart.o		\
		estrt.o ewstart.o blkcopy.o blkequ.o netutil.o		\
		udpipwr.o edcmset.o cksum.o ../../h/ethdown.h		\
		autostart.o qmapinit.o sizmem.o
		$(LD) -e estart -T ce000 -o eload			\
			startup.o eload.o eread.o ewrite.o erstart.o	\
			estrt.o ewstart.o blkcopy.o blkequ.o netutil.o	\
			udpipwr.o edcmset.o cksum.o qmapinit.o		\
			autostart.o sizmem.o

.version:	eload.c
		av.sh

clean:
		/bin/rm -rf core *.o .,* ,* libsa.a eload

depend:
		grep '^#include' *.c ../h/*.h				\
		| sed	-e 's/</"..\/h\//;s/>/"/'			\
			-e 's/:[^"]*"\([^"]*\)".*/: \1/'		\
		| sed	's/\.c/.o/' >makedep
		echo '/^# DO NOT DELETE	THIS LINE/+2,$$d' >eddep
		echo '$$r makedep' >>eddep
		echo 'w' >>eddep
		cp Makefile .,oldmake
		cp Makefile ,Makefile
		ed - ,Makefile < eddep
		/bin/rm eddep makedep
		echo '#	DEPENDENCIES MUST END AT END OF	FILE' >>,Makefile
		echo '#	IF YOU PUT STUFF HERE IT WILL GO AWAY'>>,Makefile
		echo '#	see make depend	above'>>,Makefile
		/bin/mv ,Makefile Makefile

# DO NOT DELETE	THIS LINE
# DEPENDENCIES MUST END	AT END OF FILE
blkequ.o: ../h/download.h
edcmset.o: ../h/eload.h
edcmset.o: ../h/deqna.h
eload.o: ../h/ethdown.h
eload.o: ../h/eload.h
eload.o: ../h/deqna.h
eload.o: ../h/ether.h
eload.o: ../h/network.h
eload.o: ../h/ip.h
eload.o: ../h/udp.h
eload.o: ../h/arp.h
eload.o: ../h/dlpack.h
eload.o: .version
eread.o: ../h/eload.h
eread.o: ../h/deqna.h
eread.o: ../h/ether.h
erstart.o: ../h/eload.h
erstart.o: ../h/deqna.h
erstart.o: ../h/ether.h
erstart.o: ../h/qbus.h
estrt.o: ../h/eload.h
estrt.o: ../h/deqna.h
estrt.o: ../h/ether.h
ewrite.o: ../h/eload.h
ewrite.o: ../h/deqna.h
ewrite.o: ../h/ether.h
ewstart.o: ../h/eload.h
ewstart.o: ../h/deqna.h
ewstart.o: ../h/ether.h
ewstart.o: ../h/qbus.h
getipaddr.o: ../ethdown.h
getipaddr.o: ../h/eload.h
getipaddr.o: ../h/ether.h
getipaddr.o: ../h/ip.h
getipaddr.o: ../h/arp.h
qmapinit.o: ../h/qbus.h
udpipwr.o: ../h/deqna.h
udpipwr.o: ../h/ether.h
udpipwr.o: ../h/ip.h
udpipwr.o: ../h/udp.h
udpipwr.o: ../h/eload.h
#	DEPENDENCIES MUST END AT END OF	FILE
#	IF YOU PUT STUFF HERE IT WILL GO AWAY
#	see make depend	above
