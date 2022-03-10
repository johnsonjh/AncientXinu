#
#  Make the LSI 11 downloader
#

CFLAGS  = -O
XINUDIR	= /usr/Xinu
DESTDIR	= $(XINUDIR)/bin
LIBDIR	= $(XINUDIR)/lib
CROSSAS	= $(XINUDIR)/bin/as11
INC	= $(XINUDIR)/include/sys.s
HDIR    = $(XINUDIR)/include

all:		download dl upload ul odt lusers odtunlock

install:	all
		chmod 755 download upload odt lusers odtunlock
		cp download upload odt lusers odtunlock $(DESTDIR)
		chmod 644 dl ul
		cp dl ul $(LIBDIR)

clean:
		rm -rf *.o core download upload dl ul odt lusers odtunlock

lusers:		lusers.o 
		cc $(CFLAGS) -o lusers lusers.o 

odtunlock:	odtunlock.o freedev.o
		cc $(CFLAGS) -o odtunlock odtunlock.o freedev.o

odt:		odt.o getdev.o freedev.o
		cc $(CFLAGS) -o odt odt.o getdev.o freedev.o

download:	download.o getdev.o 
		cc $(CFLAGS) -o download download.o getdev.o

upload:		upload.o getdev.o 
		cc $(CFLAGS) -o upload upload.o getdev.o

dl:		dl.s
		$(CROSSAS) -u -o dl $(INC) dl.s

ul:		ul.s
		$(CROSSAS) -u -o ul $(INC) ul.s

getdev.o:	getdev.c getdev.h
		cc $(CFLAGS) -c getdev.c

freedev.o:	freedev.c getdev.h
		cc $(CFLAGS) -c freedev.c

lusers.o: 	lusers.c getdev.h
		cc $(CFLAGS) -c lusers.c

odtunlock.o:	odtunlock.c getdev.h
		cc $(CFLAGS) -c odtunlock.c
 
odt.o:		odt.c getdev.h
		cc $(CFLAGS) -c odt.c

download.o:	download.c $(HDIR)/a.out.h getdev.h
		cc $(CFLAGS) -c download.c

upload.o:	upload.c $(HDIR)/core11.h $(HDIR)/a.out.h getdev.h
		cc $(CFLAGS) -c upload.c
