
.SUFFIXES:.o .c .s
DIR	=	/v1/burns/Xinu68
LIB	=	$(DIR)/lib
CC	=	$(DIR)/bin/mcc
CFLAGS	=	-c -I../h  -DDEBUG1 -Dmc68000
AS	=	$(DIR)/bin/as68
LD	=	$(DIR)/bin/ld68
AR	=	/bin/ar

.c.o:
		$(CC) $(CFLAGS)	$*.c
.s.o:
		$(CC) $(CFLAGS)	$*.s

CFILES	=	clkinit.c conf.c create.c 		\
		dotrace.c freemem.c			\
		getitem.c getmem.c getpid.c getstk.c	\
		ibclear.c ibfree.c ibget.c iblfree.c 	\
		ibnew.c ibput.c				\
		init.c initialize.c insert.c insertd.c	\
		ioerr.c ionull.c iosetvec.c kill.c	\
		kpollio.c				\
		lfclose.c lfgetc.c lfinit.c lfputc.c	\
		lfread.c lfsdfree.c lfseek.c lfsetup.c	\
		lfsflush.c lfsnewd.c lfwrite.c		\
		mark.c mdump.c newqueue.c		\
	 	mkpool.c poolinit.c freebuf.c getbuf.c	\
		pscfuncts.c			 	\
		queue.c ready.c receive.c		\
		recvclr.c resched.c resume.c scount.c	\
		screate.c sdelete.c send.c sendf.c	\
		signal.c signaln.c sleep.c sleep10.c	\
		sluinit.c 				\
		sreset.c ssclock.c suspend.c		\
		ttycntl.c ttygetc.c ttyiin.c ttyinit.c	\
		ttyoin.c ttyputc.c ttyread.c ttywrite.c	\
		userret.c wait.c wakeup.c xdone.c	\
		sluint.c				\
		read.c write.c init.c getc.c putc.c	\
		control.c close.c open.c seek.c 	\
		dfalloc.c dfckmd.c dfdsrch.c		\
		dscntl.c dscopy.c dsinit.c dsinter.c	\
		dskbcpy.c dskenq.c dskqopt.c dskstrt.c	\
		dsksync.c dsopen.c dsread.c dsseek.c	\
		dswrite.c dutil.c dudir.c dumkfs.c

SFILES=		clkint.s ctxsw.s ioint.s	 	\
		lowcore.s				\
		panic.s				 	\
		pssfuncts.s regdump.s			\
		setclkr.s sizmem.s startup.s

OFILES	=	clkinit.o create.o dotrace.o freemem.o	\
		getitem.o getmem.o getpid.o getstk.o	\
		ibclear.o ibfree.o ibget.o iblfree.o 	\
		ibnew.o ibput.o				\
		init.o initialize.o insert.o insertd.o	\
		ioerr.o ionull.o iosetvec.o kill.o	\
		kpollio.o				\
		lfclose.o lfgetc.o lfinit.o lfputc.o	\
		lfread.o lfsdfree.o lfseek.o lfsetup.o	\
		lfsflush.o lfsnewd.o lfwrite.o		\
		mark.o mdump.o newqueue.o		\
	 	mkpool.o poolinit.o freebuf.o getbuf.o	\
		pscfuncts.o				\
		queue.o ready.o receive.o		\
		recvclr.o resched.o resume.o scount.o	\
		screate.o sdelete.o send.o sendf.o	\
		signal.o signaln.o sleep.o sleep10.o	\
		sluinit.o	 			\
		sreset.o ssclock.o suspend.o		\
		ttycntl.o ttygetc.o ttyiin.o ttyinit.o	\
		ttyoin.o ttyputc.o ttyread.o ttywrite.o	\
		userret.o wait.o wakeup.o xdone.o	\
		sluint.o				\
		read.o write.o init.o getc.o putc.o	\
		control.o close.o open.o seek.o 	\
		dfalloc.o dfckmd.o dfdsrch.o		\
		dscntl.o dscopy.o dsinit.o dsinter.o	\
		dskbcpy.o dskenq.o dskqopt.o dskstrt.o	\
		dsksync.o dsopen.o dsread.o dsseek.o	\
		dswrite.o dutil.o dudir.o dumkfs.o	\
		clkint.o ctxsw.o ioint.o		\
		panic.o				 	\
		pssfuncts.o regdump.o			\
		setclkr.o sizmem.o startup.o


XOFILES=	startup.o lowcore.o initialize.o conf.o 

HFILES=		../h/conf.h	\
		../h/io.h	\
		../h/kernel.h	\
		../h/mem.h	\
		../h/ports.h	\
		../h/proc.h	\
		../h/q.h	\
		../h/sem.h	\
		../h/sleep.h	\
		../h/slu.h	\
		../h/tty.h

all:		libx.a xinu.o

install:
		cp libx.a $(LIB)/libx.a
		ranlib $(LIB)/libx.a
		cp xinu.o $(LIB)

clean:
		-rm -rf *.o ,* .,*
		-rm -rf libx.a a.out conf.c ../h/conf.h core make.out
		-rm -rf ../h/*.id 
		-rm -rf *.CKP ../h/*.CKP ../con/*.CKP
		
everything:	a.out xref checkup sizes lint

lint:		,lint

,lint:		$(CFILES)
		lint -n -I../h $(CFILES) | Flint > ,lint

print:
		cprint ../h/*.h *.c *.s
rprint:
		cprint $(HFILES) $(CFILES) $(SFILES)

listings:	xref sizes
		cprint Configuration ../h/*.h *.c *.s \
		    ,xref ,sizes ,ssizes Makefile
rlistings:	xref 
		cprint Configuration $(HFILES) $(CFILES) $(SFILES) \
		    ,xref Makefile

checkup:
		Checkup

sizes:		,sizes ,ssizes

,sizes:		$(OFILES) $(XOFILES) a.out
		size *.o a.out > ,sizes

,ssizes:	$(CFILES) $(SFILES) $(HFILES)
		wc   ../h/*.h `ls *.s *.c`	 > ,,ssizes
		echo '       Sorted By Name'	 > ,,ssize1
		echo '       ------ -- ----'	>> ,,ssize1
		cat  ,,ssizes			>> ,,ssize1
		echo '       Sorted By Size'	 > ,,ssize2
		echo '       ------ -- ----'	>> ,,ssize2
		sort +0n ,,ssizes		>> ,,ssize2
		pr -2 -t -m ,,ssize[12] \
		 | sed '/^$$/d'			 >  ,ssizes
		/bin/rm -rf ,,ssize[12s]



libx.a:		$(OFILES)
		/bin/rm -f  libx.a
		$(AR) cr libx.a	 $(OFILES)
		ranlib libx.a

xinu.o:		$(XOFILES)
#		$(LD) $(XOFILES) -o xinu.o

a.out:		libx.a xinu.o test.o 
		$(LD) -N  -U -T 1000 $(XOFILES) test.o libx.a $(LIB)/libxc.a libx.a 

nm:		,nm

,nm:		a.out
		Mknlist ,nm

tags:		,tags

,tags:		$(CFILES) $(HFILES)
		ctags $(CFILES)	$(HFILES)
		mv tags ,tags

xref:		,xref

,xref:		,tags
		cat ,tags | sed 's/[?^$$]//g' \
		|awk '{printf("%-15s %-15s %s %s %s %s %s	%s\n", \
		$$1,$$2,$$3,$$4,$$5,$$6,$$7,$$8)}' > ,,xref
		mv ,,xref ,xref

../h/conf.h:	
		config

conf.c:		Configuration
		config

depend:
		grep '^#include' *.c				\
		| sed	-e 's/</"..\/h\//;s/>/"/'		\
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
chprio.o: ../h/conf.h
chprio.o: ../h/kernel.h
chprio.o: ../h/proc.h
clkinit.o: ../h/conf.h
clkinit.o: ../h/kernel.h
clkinit.o: ../h/sleep.h
close.o: ../h/conf.h
close.o: ../h/kernel.h
close.o: ../h/io.h
conf.o: ../h/conf.h
control.o: ../h/conf.h
control.o: ../h/kernel.h
control.o: ../h/io.h
create.o: ../h/conf.h
create.o: ../h/kernel.h
create.o: ../h/proc.h
create.o: ../h/sem.h
create.o: ../h/mem.h
dfalloc.o: ../h/conf.h
dfalloc.o: ../h/kernel.h
dfalloc.o: ../h/disk.h
dfalloc.o: ../h/file.h
dfckmd.o: ../h/conf.h
dfckmd.o: ../h/kernel.h
dfckmd.o: ../h/disk.h
dfckmd.o: ../h/file.h
dfdsrch.o: ../h/conf.h
dfdsrch.o: ../h/kernel.h
dfdsrch.o: ../h/disk.h
dfdsrch.o: ../h/file.h
dfdsrch.o: ../h/dir.h
dldump.o: ../h/conf.h
dldump.o: ../h/kernel.h
dldump.o: ../h/slu.h
dldump.o: ../h/proc.h
dldump.o: ../h/sleep.h
dldump.o: ../h/dlc.h
dotrace.o: ../h/kernel.h
dscntl.o: ../h/conf.h
dscntl.o: ../h/kernel.h
dscntl.o: ../h/proc.h
dscntl.o: ../h/disk.h
dsinit.o: ../h/conf.h
dsinit.o: ../h/kernel.h
dsinit.o: ../h/disk.h
dsinter.o: ../h/conf.h
dsinter.o: ../h/kernel.h
dsinter.o: ../h/disk.h
dskbcpy.o: ../h/conf.h
dskbcpy.o: ../h/kernel.h
dskbcpy.o: ../h/disk.h
dskenq.o: ../h/conf.h
dskenq.o: ../h/kernel.h
dskenq.o: ../h/disk.h
dskqopt.o: ../h/conf.h
dskqopt.o: ../h/kernel.h
dskqopt.o: ../h/disk.h
dskstrt.o: ../h/conf.h
dskstrt.o: ../h/kernel.h
dskstrt.o: ../h/disk.h
dsksync.o: ../h/conf.h
dsksync.o: ../h/kernel.h
dsksync.o: ../h/proc.h
dsksync.o: ../h/disk.h
dsopen.o: ../h/conf.h
dsopen.o: ../h/kernel.h
dsopen.o: ../h/disk.h
dsopen.o: ../h/file.h
dsopen.o: ../h/dir.h
dsread.o: ../h/conf.h
dsread.o: ../h/kernel.h
dsread.o: ../h/proc.h
dsread.o: ../h/disk.h
dsseek.o: ../h/conf.h
dsseek.o: ../h/kernel.h
dsseek.o: ../h/proc.h
dsseek.o: ../h/disk.h
dswrite.o: ../h/conf.h
dswrite.o: ../h/kernel.h
dswrite.o: ../h/proc.h
dswrite.o: ../h/disk.h
dudir.o: ../h/conf.h
dudir.o: ../h/kernel.h
dudir.o: ../h/io.h
dudir.o: ../h/disk.h
dudir.o: ../h/file.h
dudir.o: ../h/dir.h
dumkfs.o: ../h/conf.h
dumkfs.o: ../h/kernel.h
dumkfs.o: ../h/disk.h
dumkfs.o: ../h/file.h
dumkfs.o: ../h/dir.h
dutil.o: ../h/conf.h
dutil.o: ../h/kernel.h
dutil.o: ../h/proc.h
dutil.o: ../h/disk.h
dutil.o: ../h/file.h
dutil.o: ../h/dir.h
dutil.o: ../h/tty.h
freebuf.o: ../h/conf.h
freebuf.o: ../h/kernel.h
freebuf.o: ../h/mark.h
freebuf.o: ../h/bufpool.h
freemem.o: ../h/conf.h
freemem.o: ../h/kernel.h
freemem.o: ../h/mem.h
getbuf.o: ../h/conf.h
getbuf.o: ../h/kernel.h
getbuf.o: ../h/mark.h
getbuf.o: ../h/bufpool.h
getc.o: ../h/conf.h
getc.o: ../h/kernel.h
getc.o: ../h/io.h
getitem.o: ../h/conf.h
getitem.o: ../h/kernel.h
getitem.o: ../h/q.h
getmem.o: ../h/conf.h
getmem.o: ../h/kernel.h
getmem.o: ../h/mem.h
getmid.o: ../h/conf.h
getmid.o: ../h/kernel.h
getmid.o: ../h/frame.h
getpid.o: ../h/conf.h
getpid.o: ../h/kernel.h
getpid.o: ../h/proc.h
getstk.o: ../h/conf.h
getstk.o: ../h/kernel.h
getstk.o: ../h/mem.h
ibclear.o: ../h/conf.h
ibclear.o: ../h/kernel.h
ibclear.o: ../h/disk.h
ibclear.o: ../h/file.h
ibclear.o: ../h/dir.h
ibfree.o: ../h/conf.h
ibfree.o: ../h/kernel.h
ibfree.o: ../h/io.h
ibfree.o: ../h/disk.h
ibfree.o: ../h/file.h
ibfree.o: ../h/dir.h
ibget.o: ../h/conf.h
ibget.o: ../h/kernel.h
ibget.o: ../h/disk.h
ibget.o: ../h/file.h
ibget.o: ../h/dir.h
iblfree.o: ../h/conf.h
iblfree.o: ../h/kernel.h
iblfree.o: ../h/io.h
iblfree.o: ../h/disk.h
iblfree.o: ../h/file.h
iblfree.o: ../h/dir.h
ibnew.o: ../h/conf.h
ibnew.o: ../h/kernel.h
ibnew.o: ../h/io.h
ibnew.o: ../h/disk.h
ibnew.o: ../h/file.h
ibnew.o: ../h/dir.h
ibput.o: ../h/conf.h
ibput.o: ../h/kernel.h
ibput.o: ../h/io.h
ibput.o: ../h/disk.h
ibput.o: ../h/file.h
ibput.o: ../h/dir.h
init.o: ../h/conf.h
init.o: ../h/kernel.h
init.o: ../h/io.h
initialize.o: ../h/conf.h
initialize.o: ../h/kernel.h
initialize.o: ../h/proc.h
initialize.o: ../h/sem.h
initialize.o: ../h/mem.h
initialize.o: ../h/tty.h
initialize.o: ../h/q.h
initialize.o: ../h/io.h
initialize.o: ../h/disk.h
insert.o: ../h/conf.h
insert.o: ../h/kernel.h
insert.o: ../h/q.h
insertd.o: ../h/conf.h
insertd.o: ../h/kernel.h
insertd.o: ../h/q.h
ioerr.o: ../h/conf.h
ioerr.o: ../h/kernel.h
ionull.o: ../h/conf.h
ionull.o: ../h/kernel.h
iosetvec.o: ../h/conf.h
iosetvec.o: ../h/kernel.h
iosetvec.o: ../h/io.h
kill.o: ../h/conf.h
kill.o: ../h/kernel.h
kill.o: ../h/proc.h
kill.o: ../h/sem.h
kill.o: ../h/mem.h
kpollio.o: ../h/kernel.h
kpollio.o: ../h/uart.h
kpollio.o: ../h/necuart.h
kpollio.o: ../h/tty.h
kprintf.o: ../h/conf.h
kprintf.o: ../h/kernel.h
kprintf.o: ../h/io.h
kprintf.o: ../h/slu.h
kprintf.o: ../h/tty.h
lfclose.o: ../h/conf.h
lfclose.o: ../h/kernel.h
lfclose.o: ../h/proc.h
lfclose.o: ../h/disk.h
lfclose.o: ../h/file.h
lfgetc.o: ../h/conf.h
lfgetc.o: ../h/kernel.h
lfgetc.o: ../h/proc.h
lfgetc.o: ../h/disk.h
lfgetc.o: ../h/file.h
lfgetc.o: ../h/dir.h
lfinit.o: ../h/conf.h
lfinit.o: ../h/kernel.h
lfinit.o: ../h/disk.h
lfinit.o: ../h/file.h
lfputc.o: ../h/conf.h
lfputc.o: ../h/kernel.h
lfputc.o: ../h/proc.h
lfputc.o: ../h/disk.h
lfputc.o: ../h/file.h
lfputc.o: ../h/dir.h
lfread.o: ../h/conf.h
lfread.o: ../h/kernel.h
lfread.o: ../h/disk.h
lfread.o: ../h/file.h
lfread.o: ../h/dir.h
lfsdfree.o: ../h/conf.h
lfsdfree.o: ../h/kernel.h
lfsdfree.o: ../h/disk.h
lfsdfree.o: ../h/file.h
lfsdfree.o: ../h/dir.h
lfseek.o: ../h/conf.h
lfseek.o: ../h/kernel.h
lfseek.o: ../h/disk.h
lfseek.o: ../h/file.h
lfseek.o: ../h/dir.h
lfsetup.o: ../h/conf.h
lfsetup.o: ../h/kernel.h
lfsetup.o: ../h/disk.h
lfsetup.o: ../h/file.h
lfsetup.o: ../h/dir.h
lfsflush.o: ../h/conf.h
lfsflush.o: ../h/kernel.h
lfsflush.o: ../h/disk.h
lfsflush.o: ../h/file.h
lfsflush.o: ../h/dir.h
lfsnewd.o: ../h/conf.h
lfsnewd.o: ../h/kernel.h
lfsnewd.o: ../h/disk.h
lfsnewd.o: ../h/file.h
lfsnewd.o: ../h/dir.h
lfwrite.o: ../h/conf.h
lfwrite.o: ../h/kernel.h
mark.o: ../h/conf.h
mark.o: ../h/kernel.h
mark.o: ../h/mark.h
mdump.o: ../h/kernel.h
mdump.o: ../h/mem.h
mkpool.o: ../h/conf.h
mkpool.o: ../h/kernel.h
mkpool.o: ../h/mark.h
mkpool.o: ../h/bufpool.h
mtest.o: ../h/kernel.h
mtest.o: ../h/conf.h
mtest.o: ../h/necuart.h
netinit.o: ../h/conf.h
netinit.o: ../h/kernel.h
netinit.o: ../h/frame.h
newqueue.o: ../h/conf.h
newqueue.o: ../h/kernel.h
newqueue.o: ../h/q.h
open.o: ../h/conf.h
open.o: ../h/kernel.h
open.o: ../h/io.h
pcount.o: ../h/conf.h
pcount.o: ../h/kernel.h
pcount.o: ../h/mark.h
pcount.o: ../h/ports.h
pcreate.o: ../h/conf.h
pcreate.o: ../h/kernel.h
pcreate.o: ../h/mark.h
pcreate.o: ../h/ports.h
pdelete.o: ../h/conf.h
pdelete.o: ../h/kernel.h
pdelete.o: ../h/mark.h
pdelete.o: ../h/ports.h
pinit.o: ../h/conf.h
pinit.o: ../h/kernel.h
pinit.o: ../h/mark.h
pinit.o: ../h/ports.h
poolinit.o: ../h/conf.h
poolinit.o: ../h/kernel.h
poolinit.o: ../h/mark.h
poolinit.o: ../h/bufpool.h
prdump.o: ../h/conf.h
prdump.o: ../h/kernel.h
prdump.o: ../h/proc.h
prdump.o: ../h/q.h
preceive.o: ../h/conf.h
preceive.o: ../h/kernel.h
preceive.o: ../h/mark.h
preceive.o: ../h/ports.h
preset.o: ../h/conf.h
preset.o: ../h/kernel.h
preset.o: ../h/mark.h
preset.o: ../h/ports.h
pscfuncts.o: ../h/kernel.h
pscfuncts.o: ../h/proc.h
psend.o: ../h/conf.h
psend.o: ../h/kernel.h
psend.o: ../h/mark.h
psend.o: ../h/ports.h
ptclear.o: ../h/conf.h
ptclear.o: ../h/kernel.h
ptclear.o: ../h/mark.h
ptclear.o: ../h/ports.h
putc.o: ../h/conf.h
putc.o: ../h/kernel.h
putc.o: ../h/io.h
qdump.o: ../h/conf.h
qdump.o: ../h/kernel.h
qdump.o: ../h/proc.h
qdump.o: ../h/q.h
queue.o: ../h/conf.h
queue.o: ../h/kernel.h
queue.o: ../h/q.h
read.o: ../h/conf.h
read.o: ../h/kernel.h
read.o: ../h/io.h
ready.o: ../h/conf.h
ready.o: ../h/kernel.h
ready.o: ../h/proc.h
ready.o: ../h/q.h
receive.o: ../h/conf.h
receive.o: ../h/kernel.h
receive.o: ../h/proc.h
recvclr.o: ../h/conf.h
recvclr.o: ../h/kernel.h
recvclr.o: ../h/proc.h
resched.o: ../h/conf.h
resched.o: ../h/kernel.h
resched.o: ../h/proc.h
resched.o: ../h/q.h
resume.o: ../h/conf.h
resume.o: ../h/kernel.h
resume.o: ../h/proc.h
scount.o: ../h/conf.h
scount.o: ../h/kernel.h
scount.o: ../h/sem.h
screate.o: ../h/conf.h
screate.o: ../h/kernel.h
screate.o: ../h/proc.h
screate.o: ../h/q.h
screate.o: ../h/sem.h
sdelete.o: ../h/conf.h
sdelete.o: ../h/kernel.h
sdelete.o: ../h/proc.h
sdelete.o: ../h/q.h
sdelete.o: ../h/sem.h
seek.o: ../h/conf.h
seek.o: ../h/kernel.h
seek.o: ../h/io.h
send.o: ../h/conf.h
send.o: ../h/kernel.h
send.o: ../h/proc.h
sendf.o: ../h/conf.h
sendf.o: ../h/kernel.h
sendf.o: ../h/proc.h
signal.o: ../h/conf.h
signal.o: ../h/kernel.h
signal.o: ../h/proc.h
signal.o: ../h/q.h
signal.o: ../h/sem.h
signaln.o: ../h/conf.h
signaln.o: ../h/kernel.h
signaln.o: ../h/proc.h
signaln.o: ../h/q.h
signaln.o: ../h/sem.h
sleep.o: ../h/conf.h
sleep.o: ../h/kernel.h
sleep.o: ../h/proc.h
sleep.o: ../h/q.h
sleep.o: ../h/sleep.h
sleep10.o: ../h/conf.h
sleep10.o: ../h/kernel.h
sleep10.o: ../h/proc.h
sleep10.o: ../h/q.h
sleep10.o: ../h/sleep.h
sluinit.o: ../h/conf.h
sluinit.o: ../h/kernel.h
sluinit.o: ../h/tty.h
sluinit.o: ../h/io.h
sluint.o: ../h/conf.h
sluint.o: ../h/kernel.h
sluint.o: ../h/tty.h
sluint.o: ../h/io.h
sluint.o: ../h/necuart.h
sreset.o: ../h/conf.h
sreset.o: ../h/kernel.h
sreset.o: ../h/proc.h
sreset.o: ../h/q.h
sreset.o: ../h/sem.h
ssclock.o: ../h/conf.h
ssclock.o: ../h/kernel.h
ssclock.o: ../h/proc.h
ssclock.o: ../h/q.h
ssclock.o: ../h/sleep.h
suspend.o: ../h/conf.h
suspend.o: ../h/kernel.h
suspend.o: ../h/proc.h
tdump.o: ../h/conf.h
tdump.o: ../h/kernel.h
tdump.o: ../h/tty.h
test.o: ../h/conf.h
test.o: ../h/kernel.h
test.o: ../h/disk.h
ttycntl.o: ../h/conf.h
ttycntl.o: ../h/kernel.h
ttycntl.o: ../h/tty.h
ttycntl.o: ../h/io.h
ttygetc.o: ../h/conf.h
ttygetc.o: ../h/kernel.h
ttygetc.o: ../h/tty.h
ttygetc.o: ../h/io.h
ttyiin.o: ../h/conf.h
ttyiin.o: ../h/kernel.h
ttyiin.o: ../h/tty.h
ttyiin.o: ../h/io.h
ttyinit.o: ../h/conf.h
ttyinit.o: ../h/kernel.h
ttyinit.o: ../h/tty.h
ttyinit.o: ../h/io.h
ttyoin.o: ../h/conf.h
ttyoin.o: ../h/kernel.h
ttyoin.o: ../h/tty.h
ttyoin.o: ../h/io.h
ttyoin.o: ../h/necuart.h
ttyputc.o: ../h/conf.h
ttyputc.o: ../h/kernel.h
ttyputc.o: ../h/proc.h
ttyputc.o: ../h/tty.h
ttyputc.o: ../h/io.h
ttyread.o: ../h/conf.h
ttyread.o: ../h/kernel.h
ttyread.o: ../h/tty.h
ttyread.o: ../h/io.h
ttywrite.o: ../h/conf.h
ttywrite.o: ../h/kernel.h
ttywrite.o: ../h/tty.h
ttywrite.o: ../h/io.h
userret.o: ../h/conf.h
userret.o: ../h/kernel.h
wait.o: ../h/conf.h
wait.o: ../h/kernel.h
wait.o: ../h/proc.h
wait.o: ../h/q.h
wait.o: ../h/sem.h
wakeup.o: ../h/conf.h
wakeup.o: ../h/kernel.h
wakeup.o: ../h/proc.h
wakeup.o: ../h/q.h
wakeup.o: ../h/sleep.h
write.o: ../h/conf.h
write.o: ../h/kernel.h
write.o: ../h/io.h
#	DEPENDENCIES MUST END AT END OF	FILE
#	IF YOU PUT STUFF HERE IT WILL GO AWAY
#	see make depend	above
