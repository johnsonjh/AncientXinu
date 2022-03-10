/*----------------------------------------------------------------------*/
/*									*/
/*	download	+++ load programs into MC 68000 +++		*/
/*									*/
/* This program takes MC68000 a.out format files and downloads them	*/
/* into a Pacific Microcomputer 68K					*/
/*									*/
/* Usage:								*/
/*	download [filename] [-t tty] [-b filename] [-v] [-s]		*/
/*									*/
/* Author:								*/
/*      D. Burns                                                        */
/*	Princeton University/EECS Deptartment				*/
/*	July, 1983							*/
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <a.out.h>
#include "define.h"

FILE	*logfp;
struct	options A;
int	alarmhandler();
int	inthandler();
int	ttyset	=	FALSE;
int	fastlength;

main ( argc, argv )
	int	argc;
	char	*argv[];
{
	FILE	*bootfp, *fastfp, *userfp, *fopen();
	struct	exec	boothdr, fasthdr, userhdr;
	char	buf[32];

	/*
	 * set up terminal modes...prepare to reset on interrupt
	 */
	procargs(argc, argv);

	if ( (logfp  = fopen(A.logfile,    "w")) == NULL) {
		fprintf(stderr, "Cannot open file: %s\n", A.logfile);
		exit(-1);
	}
	if ( (bootfp = fopen(A.bootloader, "r")) == NULL)
		error("Cannot open file: %s\n", A.bootloader);
	if ( (fastfp = fopen(A.fastloader, "r")) == NULL)
		error("Cannot open file: %s\n", A.fastloader);
	if ( (userfp = fopen(A.userprog,   "r")) == NULL)
		error("Cannot open file: %s\n", A.userprog);

	signal(SIGINT,inthandler);
	signal(SIGTERM,inthandler);
	signal(SIGQUIT,inthandler);
	signal(SIGALRM, alarmhandler);

        /*lock();*/
	gethdr(fastfp, &fasthdr, A.fastloader);
	gethdr(userfp, &userhdr, A.userprog);
	getmon();
	loadboot(bootfp);
        fastlength = loadfast(fastfp, &fasthdr);
	loadprog(userfp, &userhdr, A.loadaddr, fastlength);
        message("Done");
	autostart();
	callexit(0);
}
