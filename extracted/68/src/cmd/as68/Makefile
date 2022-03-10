DESTDIR	=	/v1/burns/Xinu68/bin
FILES	=	mical.h inst.h scan.h MegaStab.h scan.c error.c init.c \
        	ins.c ps.c rel.c sdi.c sym.c b.out.h MegaStab.c

OBJECTS =	MegaStab.o error.o init.o ins.o ps.o rel.o sdi.o sym.o scan.o

CFLAGS =	-g

as68:		$(OBJECTS)
		cc -o as68 $(OBJECTS)
		rm -f $(DESTDIR)/as68
		cp as68 $(DESTDIR)/as68

print:
		cprint  ${FILES}

$(OBJECTS):	mical.h

init.o ins.o: 	mical.h inst.h

mical.h :	scan.h

rel.c:		mical.h b.out.h

sym.c:		mical.h b.out.h

MegaStab.c:	MegaStab.h scan.h
