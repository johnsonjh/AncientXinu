
.SUFFIXES:.o .c .s
DIR	=	/v1/burns/Xinu68
LIB	=	$(DIR)/lib
MCC	=	$(DIR)/bin/mcc
MCFLAGS	=	-c  -DDEBUG1 -Dmc68000
MAS	=	$(DIR)/bin/as68
MLD	=	$(DIR)/bin/ld68
MAR	=	/bin/ar

CFILES	=	exit.c gethdr.c getmon.c loadboot.c		\
		loadfast.c loadprog.c main.c message.c		\
		procargs.c readuntil.c sendmon.c sendpack.c	\
		setreg.c startmon.c unctrl.c usagexit.c		\
		autostart.c

SFILES	=	boot.s fast.s

OFILES	=	exit.o gethdr.o getmon.o loadboot.o		\
		loadfast.o loadprog.o main.o message.o		\
		procargs.o readuntil.o sendmon.o sendpack.o	\
		setreg.o startmon.o unctrl.o usagexit.o 	\
		autostart.o

CFLAGS	=	-g


$(DIR)/bin/download: download boot.68 fast.o
		cp download $(DIR)/bin/download

download:	$(OFILES)
		cc $(CFLAGS) -o download $(OFILES)

fast.o:		fast.s
		/lib/cpp fast.s | tail +1  >temp.s
		as68 -o fast.o temp.s
		rm temp.s

boot.o:		boot.s
		as68 -o boot.o boot.s

boot.68:	boot.o srec
		srec boot.o > boot.68

srec:		srec.o
		cc -o srec srec.o

print:		$(CFILES) $(SFILES)
		imprint $(CFILES) $(SFILES)

depend:
		grep '^#include' *.c				\
		| sed	-e 's/</"\/usr\/include\//;s/>/"/'		\
			-e 's/:[^"]*"\([^"]*\)".*/: \1/'	\
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
exit.o: /usr/include/sys/ioctl.h
exit.o: /usr/include/signal.h
exit.o: define.h
gethdr.o: define.h
getmon.o: /usr/include/sgtty.h
getmon.o: define.h
loadboot.o: define.h
loadfast.o: /usr/include/a.out.h
loadfast.o: define.h
loadprog.o: define.h
main.o: /usr/include/stdio.h
main.o: /usr/include/signal.h
main.o: /usr/include/errno.h
main.o: /usr/include/a.out.h
main.o: define.h
message.o: define.h
procargs.o: define.h
readuntil.o: /usr/include/stdio.h
readuntil.o: /usr/include/errno.h
readuntil.o: define.h
sendmon.o: define.h
sendpack.o: define.h
setreg.o: define.h
setreg.o: regdefs.h
#	DEPENDENCIES MUST END AT END OF	FILE
#	IF YOU PUT STUFF HERE IT WILL GO AWAY
#	see make depend	above
