/*
 *              V A X  -  T O  -  T U 5 8   C O P Y
 *
 * use:
 * dd58 [[-]verify] [[-]sam] [[-]maint] [count=c] [drive=d] [posit=p] action
 * where:
 * verify turns on verification mode (read and written data is checked)
 * sam    sets special addressing mode (address blocks of 128 bytes)
 * maint  turns on maintenance mode (no retries on data errors)
 * [A prefixed "-" turns the option off]
 * c      number of bytes to be used [MUST be present for read, defaults to
 *        the input size on write]
 * d      drive to be used
 * p      block number at which action is to commence
 * action one of:
 *        read          read from the TU58
 *        write         write to the TU58
 *        seek          position the TU58
 */

/*
 * useful goodies
 */
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include "/usr/Xinu/include/tu58io.h"
#include "/usr/Xinu/include/tu58errno.h"

/*
 * the defaults
 */
#define DEFCOUNT        (2048 * 128 + 1)        /* whole tape + 1 byte */
#define DEFDRIVE        "0"
#define DEFBLOCK        0
#define DEFBSIZ         512
#define DEFVFY          1
#define DEFMAINT        0
#define min(x,y)        ((x)>(y))?(y):(x)

/*
 * variables
 */
extern int terrno;              /* tape error number */
int count = DEFCOUNT;           /* number of bytes to be used */
char *drive = DEFDRIVE;         /* drive number */
int posit = DEFBLOCK;           /* drive position */
int blksiz = DEFBSIZ;           /* size of blocks (128 vs. 512) */
int verify = DEFVFY;            /* verification mode */
int maint = DEFMAINT;           /* maint mode */
int readflg = 0;                /* 1 for reading */
int writeflg = 0;               /* 1 for writing */
int seekflg = 0;                /* 1 for seeking */
int dn;                         /* drive descriptor */

/*
 * and we're off!
 */
main(argc, argv)
int argc;
char **argv;
{
        int i;                  /* counter in a for loop */

        /* walk arg list, setting parameters */
        for(i = 1; i < argc; i++)
                if (match(argv[i], "sam"))
                        blksiz = 128;
                else if (match(argv[i], "-sam"))
                        blksiz = 512;
                else if (match(argv[i], "verify"))
                        verify = 1;
                else if (match(argv[i], "-verify"))
                        verify = 0;
                else if (match(argv[i], "maint"))
                        maint = 1;
                else if (match(argv[i], "-maint"))
                        maint = 0;
                else if (match(argv[i], "drive")){
                        if ((drive = argv[i] + 5 + (argv[i][5] == '=')) == '\0')
                                drive = DEFDRIVE;
                }
                else if (match(argv[i], "posit"))
                        posit = atoi(argv[i] + 5 + (argv[i][5] == '='));
                else if (match(argv[i], "count"))
                        count = atoi(argv[i] + 5 + (argv[i][5] == '='));
                else if (match(argv[i], "read"))
                        readflg = 1;
                else if (match(argv[i], "write"))
                        writeflg = 1;
                else if (match(argv[i], "seek"))
                        seekflg = 1;
                else
                        fprintf(stderr, "%s: unknown option; ignored\n", argv[0]);

        /* exactly ONE action MUST be specified */
        if (readflg + writeflg + seekflg < 1){
                fprintf(stderr, "no action requested\n");
                exit(1);
        }
        else if (readflg + writeflg + seekflg > 1){
                fprintf(stderr, "inconsistent actions requested\n");
                exit(1);
        }

        /* open the drive ... */
        if (readflg)    /* ... for reading */
                if ((dn = topen(drive, 0, (char *) NULL)) < 0){
                        tperror("error opening TU58");
                        exit(1);
                }
                else if (count == DEFCOUNT){
                        fprintf(stderr, "no count given\n");
                        exit(1);
                }
                else{
                        /* go ! */
                        setioctl();
                        readtape(fileno(stdout));
                }
        else if (writeflg)      /* ... for writing */
                if ((dn = topen(drive, 1, (char *) NULL)) < 0){
                        tperror("error opening TU58");
                        exit(1);
                }
                else{
                        /* go ! */
                        setioctl();
                        writetape(fileno(stdin));
                }
        else if (seekflg)       /* ... for seeking */
                if ((dn = topen(drive, 2, (char *) NULL)) < 0){
                        tperror("error opening TU58");
                        exit(1);
                }
                else{
                        /* go ! */
                        setioctl();
                        seektape();
                }

        /* close the tape */
        if (tclose(dn) < 0)
                tperror("error closing TU58");
        exit(0);

}

/*
 * this sets the required parameters
 */
setioctl()
{
        /* position the drive (quicker than a seek) */
        if (tioctl(dn, TU58SPOS, &posit) < 0){
                tperror("error setting drive position");
                exit(1);
        }
        /* set special address mode */
        if (tioctl(dn, (blksiz == 128) ? TU58SSAM : TU58CSAM, NULL) < 0){
                tperror("error setting special address mode");
                exit(1);
        }
        /* set verify mode */
        if (tioctl(dn, verify ? TU58SVFY : TU58CVFY, NULL) < 0){
                tperror("error setting verify mode");
                exit(1);
        }
        /* set maintenance mode */
        if (tioctl(dn, maint ? TU58SMTM : TU58CMTM, NULL) < 0){
                tperror("error setting maintenance mode");
                exit(1);
        }
}

/*
 * this reads the tape
 */
readtape(fd)
int fd;                 /* output file descriptor */
{
        char buf[1024];         /* buffer for input */
        int n;                  /* number of bytes read */
        int ct;                 /* number of bytes remaining to be read */
        int x;                  /* number of bytes to be read this loop */

        /* your usual plug-and-chug loop */
        for(ct = count; ct > 0; ct -= x){
                x = min(blksiz, ct);
                if ((n = tread(dn, buf, x)) != x || terrno != TENONE)
                        tperror("dd58");
                else if (write(fd, buf, n) != n)
                        perror("dd58");
        }
        fprintf(stderr, "%d bytes, %d blocks read from block %d",
                                count-ct, (count-ct+blksiz-1)/blksiz, posit);
        fprintf(stderr, " (%d bytes to a block)\n", blksiz);
}

/*
 * this writes the tape
 */
writetape(fd)
int fd;                 /* output file descriptor */
{
        char buf[1024];         /* buffer for input */
        int n;                  /* number of bytes read */
        int ct;                 /* number of bytes remaining to be read */
        int x;                  /* number of bytes to be read on next loop */

        for(ct = 0; ct < count && (n = read(fd, buf, blksiz)) > 0; ct += n)
                if (twrite(dn, buf, n) != n || terrno != TENONE)
                        tperror("dd58");

        fprintf(stderr, "%d bytes, %d blocks written from block %d",
                                ct, (ct+blksiz-1)/blksiz, posit);
        fprintf(stderr, " (%d bytes to a block)\n", blksiz);
}

/*
 * this positions the tape
 */
seektape()
{
        /* try to position the tape */
        if (tseek(dn, posit, 0) != posit)
                perror("error positioning TU58");
        else{
                /* print "all done" message */
                fprintf(stderr, "drive %d positioned at block %d", dn, posit);
                fprintf(stderr, " (%d bytes to a block)\n", blksiz);
        }
}

/*
 * this matches patterns
 */
int match(a, b)
char *a, *b;                    /* return 1 if a matches b */
{
        while(*a && *a != '=' && *b++ == *a++);

        return(*a == '=' || !*b);
}
