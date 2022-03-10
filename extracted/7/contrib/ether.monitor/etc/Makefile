#
# Makefile for pseudo mshell and associated commands
#

.SUFFIXES:.o .c .s
DIR	=	/usr/Xinu
BINDIR	=	$(DIR)/bin
CC	=	$(BINDIR)/cc11
LD	=	$(BINDIR)/ld11
CFLAGS	=	-O -c -I../h
.c.o:
		$(CC) $(CFLAGS)	$<
.s.o:
		$(CC) $(CFLAGS) $<

CFILES	=	margparse.c mtabinit.c	\
		minit.c meread.c mewrite.c mshell.c		\
		mep.c mip.c marp.c mrarp.c			\
		mudp.c micmp.c mtcp.c mhelp.c			


all:		monitor

monitor:	mshell.c margparse.o mtabinit.o	\
		minit.o meread.o mewrite.o 		\
		mep.o mip.o marp.o mrarp.o			\
		mudp.o micmp.o mtcp.o mhelp.o				
		$(CC) -o monitor mshell.c  \
			 margparse.o mtabinit.o	\
			minit.o meread.o mewrite.o 	\
			mep.o mip.o marp.o mrarp.o		\
			mudp.o micmp.o mtcp.o mhelp.o			

install:	monitor
		cp monitor $(BINDIR)

clean:
		-rm -rf *.o ,* .,* core make.out

# DO NOT DELETE	THIS LINE
# DEPENDENCIES MUST END	AT END OF FILE
mshell.c: ../h/conf.h
mshell.c: ../h/kernel.h
mshell.c: ../h/proc.h
mshell.c: ../h/tty.h
mshell.c: ../h/slu.h
mshell.c: ../h/deqna.h
mshell.c: ../h/ether.h
mshell.c: ../h/ctype.h
mshell.c: ../h/tcp.h
mshell.c: ../h/network.h
mshell.c: ../h/monitor.h
mshell.c: ../h/mshell.h
mshell.c: ../h/mshellc.h
mparse.o: ../h/conf.h
mparse.o: ../h/kernel.h
mparse.o: ../h/mshell.h
mparse.0: ../h/mark.h
mtabinit.o: ../h/conf.h
mtabinit.o: ../h/kernel.h
mtabinit.o: ../h/proc.h
mtabinit.o: ../h/deqna.h
mtabinit.o: ../h/ether.h
mtabinit.o: ../h/ip.h
mtabinit.o: ../h/arp.h
mtabinit.o: ../h/net.h
mtabinit.o: ../h/network.h
mtabinit.o: ../h/monitor.h
margparse.o: ../h/conf.h
margparse.o: ../h/kernel.h
margparse.o: ../h/proc.h
margparse.o: ../h/deqna.h
margparse.o: ../h/ether.h
margparse.o: ../h/ip.h
margparse.o: ../h/ctype.h
margparse.o: ../h/monitor.h
minit.o: ../h/kernel.h
minit.o: ../h/deqna.h
minit.o: ../h/ether.h
minit.o: ../h/slu.h
minit.o: ../h/network.h
meread.o: ../h/kernel.h
meread.o: ../h/conf.h
meread.o: ../h/deqna.h
meread.o: ../h/ether.h
mewrite.o: ../h/kernel.h
mewrite.o: ../h/deqna.h
mewrite.o: ../h/ether.h
mep.o: ../h/conf.h
mep.o: ../h/kernel.h
mep.o: ../h/deqna.h
mep.o: ../h/ether.h
mep.o: ../h/ip.h
mep.o: ../h/arp.h
mep.o: ../h/ctype.h
mep.o: ../h/network.h
mep.o: ../h/monitor.h
mip.o: ../h/conf.h
mip.o: ../h/kernel.h
mip.o: ../h/ctype.h
mip.o: ../h/deqna.h
mip.o: ../h/ether.h
mip.o: ../h/ip.h
mip.o: ../h/network.h
mip.o: ../h/monitor.h
marp.o: ../h/conf.h
marp.o: ../h/kernel.h
marp.o: ../h/deqna.h
marp.o: ../h/ether.h
marp.o: ../h/ip.h
marp.o: ../h/arp.h
marp.o: ../h/network.h
marp.o: ../h/monitor.h
mrarp.o: ../h/conf.h
mrarp.o: ../h/kernel.h
mrarp.o: ../h/deqna.h
mrarp.o: ../h/ether.h
mrarp.o: ../h/ip.h
mrarp.o: ../h/arp.h
mrarp.o: ../h/network.h
mrarp.o: ../h/monitor.h
mudp.o: ../h/conf.h
mudp.o: ../h/kernel.h
mudp.o: ../h/deqna.h
mudp.o: ../h/ether.h
mudp.o: ../h/ip.h
mudp.o: ../h/udp.h
mudp.o: ../h/network.h
mudp.o: ../h/monitor.h
micmp.o: ../h/conf.h
micmp.o: ../h/kernel.h
micmp.o: ../h/ctype.h
micmp.o: ../h/deqna.h
micmp.o: ../h/ether.h
micmp.o: ../h/ip.h
micmp.o: ../h/icmp.h
micmp.o: ../h/network.h
micmp.o: ../h/monitor.h
mtcp.o: ../h/conf.h
mtcp.o: ../h/kernel.h
mtcp.o: ../h/deqna.h
mtcp.o: ../h/ether.h
mtcp.o: ../h/ip.h
mtcp.o: ../h/tcp.h
mtcp.o: ../h/network.h
mtcp.o: ../h/monitor.h
mhelp.o: ../h/conf.h
mhelp.o: ../h/kernel.h
mhelp.o: ../h/mshell.h
mhelp.o: ../h/mshellc.h
netnum.o: ../h/conf.h
netnum.o: ../h/kernel.h
netnum.o: ../h/ip.h
blkcopy.o: ../h/kernel.h
blkequ.o: ../h/kernel.h
arpfind.o: ../h/conf.h
arpfind.o: ../h/kernel.h
arpfind.o: ../h/proc.h
arpfind.o: /usr/Xinu/include/network.h
#	DEPENDENCIES MUST END AT END OF	FILE
#	IF YOU PUT STUFF HERE IT WILL GO AWAY
#	see make depend	above
