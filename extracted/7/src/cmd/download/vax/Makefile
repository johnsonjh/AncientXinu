#
# Makefile for VAX-specific downloading routines
#

all:
		@echo Making VAX-resident portion of up/downloaders
		(cd eload ; make all)
		@echo Making UNIX portion of VAX up/downloaders
		(cd src   ; make all)

install:
		@echo Making and installing VAX-resident portion of up/downloaders
		(cd eload ; make install)
		@echo Making and installing UNIX portion of VAX up/downloaders
		(cd src   ; make install)

clean:
		@echo Cleaning VAX-resident portion up/downloader directory
		(cd eload ; make clean)
		@echo Cleaning VAX UNIX portion up/downloader directory
		(cd src   ; make clean)

