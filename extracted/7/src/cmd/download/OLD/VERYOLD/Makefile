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

all:		download dl upload ul odt lusers LSIunlock

install:	all
		chmod 755 download upload odt lusers LSIunlock
		cp download upload odt lusers LSIunlock $(DESTDIR)
		chmod 644 dl ul
		cp dl ul $(LIBDIR)

clean:
		rm -rf *.o core download upload dl ul odt

lusers:		lusers.o 
		cc $(CFLAGS) -o lusers lusers.o 

LSIunlock:	LSIunlock.o 
		cc $(CFLAGS) -o LSIunlock LSIunlock.o 

odt:		odt.c getlsi.o
		cc $(CFLAGS) -o odt odt.c getlsi.o

download:	download.o getlsi.o 
		cc $(CFLAGS) -o download download.o getlsi.o

upload:		upload.o getlsi.o 
		cc $(CFLAGS) -o upload upload.o getlsi.o

dl:		dl.s
		$(CROSSAS) -u -o dl $(INC) dl.s

ul:		ul.s
		$(CROSSAS) -u -o ul $(INC) ul.s

getlsi.o:	getlsi.c $(HDIR)/getlsi.h $(HDIR)/lsidevs.h
		cc $(CFLAGS) -c getlsi.c

lusers.o: 	lusers.c $(HDIR)/getlsi.h $(HDIR)/lsidevs.h
		cc $(CFLAGS) -c lusers.c

LSIunlock.o:	LSIunlock.c $(HDIR)/getlsi.h $(HDIR)/lsidevs.h
		cc $(CFLAGS) -c LSIunlock.c
 
download.o:	download.c $(HDIR)/a.out.h $(HDIR)/getlsi.h
		cc $(CFLAGS) -c download.c

upload.o:	upload.c $(HDIR)/core11.h $(HDIR)/a.out.h $(HDIR)/getlsi.h
		cc $(CFLAGS) -c upload.c
