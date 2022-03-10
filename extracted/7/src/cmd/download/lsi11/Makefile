#
# Makefile for Lsi11-specific downloading routines
#

all:
		@echo Making Lsi-11 resident portion of up/downloaders
		(cd eload ; make all)
		@echo Making UNIX portion of Lsi-11 up/downloaders
		(cd src   ; make all)

install:
		@echo Making and installing Lsi-11 resident portion of up/downloaders
		(cd eload ; make install)
		@echo Making and installing UNIX portion of Lsi-11 up/downloaders
		(cd src   ; make install)

clean:
		@echo Cleaning LSI-11 resident portion up/downloader directory
		(cd eload ; make clean)
		@echo Cleaning LSI-11 UNIX portion up/downloader directory
		(cd src   ; make clean)

