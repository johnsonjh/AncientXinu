#
# Makefile for Xinu downloader library routines, including database
# reservation routines
#

HDIR	= ../h
CFLAGS  = -O -I$(HDIR)
DIR	= /usr/Xinu
AR	= /bin/ar

all:		libd.a

install:	all

clean:
		/bin/rm -rf *.o ,* .,* core a.out libd.a

libd.a:		freedev.o getdb.o getdev.o ulock.o usleep.o
		rm -rf libd.a
		$(AR) cr libd.a freedev.o getdb.o getdev.o ulock.o usleep.o
		ranlib libd.a

freedev.o:	freedev.c $(HDIR)/freedev.h $(HDIR)/getdev.h
		cc  $(CFLAGS) -c freedev.c

getdb.o:	getdb.c $(HDIR)/getdev.h
		cc  $(CFLAGS) -c getdb.c

getdev.o:	getdev.c $(HDIR)/getdev.h $(HDIR)/freedev.h $(HDIR)/ulock.h
		cc  $(CFLAGS) -c getdev.c

ulock.o:	ulock.c $(HDIR)/ulock.h $(HDIR)/getdev.h $(HDIR)/freedev.h
		cc  $(CFLAGS) -c ulock.c

usleep.o:	usleep.c
		cc $(CFLAGS) -c usleep.c
