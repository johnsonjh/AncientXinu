CFILES	=	abs.c atof.c atoi.c atol.c		\
		ctype_.c doprnt.c doscan.c ecvt.c	\
		fgets.c fprintf.c fputs.c gets.c	\
		index.c printf.c puts.c qsort.c		\
		rand.c rindex.c scanf.c sprintf.c	\
		strcat.c strcmp.c strcpy.c strlen.c	\
		strncat.c strncmp.c strncpy.c swab.c	

SFILES	=	aldiv.s almul.s alrem.s auldiv.s	\
		aulmul.s aulrem.s blt.s clear.s		\
		float.s ldiv.s lmul.s lrem.s 		\
		mcount.s setjmp.s uldiv.s ulmul.s 	\
		ulrem.s

OFILES	=	abs.o aldiv.o almul.o alrem.o		\
		atof.o atoi.o atol.o auldiv.o		\
		aulmul.o aulrem.o blt.o clear.o		\
		ctype_.o doprnt.o doscan.o ecvt.o	\
		fgets.o float.o fprintf.o fputs.o	\
		gets.o index.o ldiv.o lmul.o		\
		lrem.o mcount.o printf.o puts.o		\
		qsort.o rand.o rindex.o scanf.o		\
		setjmp.o sprintf.o strcat.o strcmp.o	\
		strcpy.o strlen.o strncat.o strncmp.o	\
		strncpy.o swab.o uldiv.o ulmul.o	\
		ulrem.o	

DIR	=	/v1/burns/Xinu68
LIB	=	$(DIR)/lib
CC	=	$(DIR)/bin/mcc
LD	=	$(DIR)/bin/ld68
AS	=	$(DIR)/bin/as68
CFLAGS	=	-I$(DIR)/src/sys/h

.c.o:
		$(CC) $(CFLAGS) -c $*.c
		-$(LD) -x -r $*.o
		mv a.out $*.o

.s.o:
		$(AS) -o $*.o $*.s
		-$(LD) -U -x -r $*.o
		mv a.out $*.o

all:		${OFILES}
		rm -f libxc.a
		ar cr libxc.a $(OFILES)
		ranlib libxc.a
		cp libxc.a $(LIB)/libxc.a
		
clean:
		rm -f ${OFILES}
