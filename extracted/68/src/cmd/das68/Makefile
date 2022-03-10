TCFILES	=	addtext.c biti.c extend.c initialize.c		\
		instfetch.c main.c mapsize.c movesr.c		\
		mystrlen.c nlistcomp.c obranch.c ochk.c		\
		odbcc.c oimmed.c olink.c omove.c		\
		omovem.c omoveq.c oneop.c opmode.c		\
		oprint.c oquick.c oreg.c oscc.c			\
		otrap.c pexact.c printEA.c			\
		printea.c printins.c shroi.c soneop.c		\
		suffix.c sxtword.c

TOFILES	=	addtext.o biti.o extend.o initialize.o 		\
		instfetch.o main.o mapsize.o movesr.o		\
		mystrlen.o nlistcomp.o obranch.o ochk.o		\
		odbcc.o oimmed.o olink.o omove.o		\
		omovem.o omoveq.o oneop.o opmode.o		\
		oprint.o oquick.o oreg.o oscc.o			\
		otrap.o pexact.o printEA.o			\
		printea.o printins.o shroi.o soneop.o		\
		suffix.o sxtword.o

DIR	=	/v1/burns/Xinu68/bin

all:		ntext 

ntext:		$(TOFILES)
		cc -g -o ntext $(TOFILES)
		cp ntext $(DIR)
