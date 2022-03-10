/* LINTLIBRARY */
/*
 * This package was written by Matt Bishop of the CS Department at Purdue
 * University, in December 1981, for Doug Comer's 690D seminar.
 *
 * this sets up useful goodies. stdio.h is NOT used, but some ioctl calls
 * need NULL. Hence NULL is defined here for the vax; for anything else
 * you can include stdio.h.
 */
#ifndef NULL
#define	NULL	0
#endif

#include <sgtty.h>
#include <errno.h>
#include <tu58.h>

/*
 * this rounds x up to the nearest y; useful for computing number of blocks
 * written out
 */
#define nblock(x,y)             ((x) + (y) - 1) / (y)

/*
 * globals. tape58 is the structure for the TU58; abytes is the number of
 * bytes affected by the last io operation, as returned by the end packet.
 * errno is the standard error number for UNIX errors, and terrno is the
 * corresponding error number for the tape routines.
 */
static struct tu58 tape58;              /* TU58 control structure */
static int abytes;                      /* number of bytes handled last */
extern int errno;                       /* UNIX error indicator */
int terrno;                             /* global TU58 errors */

/*
 * topen(drive, mode, name)
 * char *drive;                 first char is number of drive being opened
 * int mode;                    0 for reading, 1 for writing, 2 for both
 * char *name;                  name of device to be opened
 *
 * note the device "name" (defaulting to the standard TU58FILE) is opened
 * for reading and writing regardless of the mode; the mode is used with
 * the drives. you need to go both ways to send, and recieve, information
 * from the TU58.
 * also note the existence of the lock file; to dump it, you must call
 * tclose.
 *
 * RETURNS: -1 on error; terrno indicates what happened
 *          drive number on success
 */
int topen(drive, mode, name)
char *drive;                    /* drive being opened */
int mode;                       /* mode (reading or writing or both) */
char *name;                     /* Alternate device name */
{
        int x;                  /* temp for initialization error */
        int td;                 /* temp file descriptor */
        int dn;                 /* drive number */
        struct dr58 *ds;        /* pointer to drive structure */
        struct sgttyb tsbuf;    /* temp for sgttyb block */
        struct tchars tcbuf;    /* temp for tchars block */

        /* check drive number */
        if (*drive != '0' && *drive != '1'){
                terrno = TEBUNN;
                return(SYSERR);
        }

        /* check mode number */
        if (mode < 0 || mode > 2){
                terrno = EINVAL - TEUNIX;
                return(SYSERR);
        }

        /* see if any drive is open */
        ds = &(tape58 . t_dr[dn = *drive - '0']);
        if (tape58 . t_open > 0){
                /* yes - check drive and if closed, open it */
                if (ds -> d_flags & OPEN){
                        terrno = TEREOP;
                        return(dn);
                }
                ds -> d_flags = OPEN | (mode ? WRITE : 0) | ((mode & 01) ? 0 : READ);
                ds -> d_posit = 0;
                ds -> d_mods = (char) VFY;
                ds -> d_swit = '\0';
                return(dn);
        }

        /* no drive is open - open the file and set its modes */
        if (creat(TU58LOCK, 0600) < 0){
                terrno = TETULU;
                return(SYSERR);
        }
        if (name == NULL)
                name = TU58FILE;
        if ((tape58 . t_fd = td = open(name, 2)) < 0){
                terrno = errno - TEUNIX;
                (void) unlink(TU58LOCK);
                return(SYSERR);
        }
        ds -> d_flags = OPEN | (mode ? WRITE : 0) | ((mode & 01) ? 0 : READ);
        ds -> d_posit = 0;
        ds -> d_mods = (char) VFY;
        ds -> d_swit = '\0';

        /* reset start, stop chars */
        if (ioctl(td, TIOCGETC, &tcbuf) < 0){
                terrno = errno - TEUNIX;
                (void) close(td);
                (void) unlink(TU58LOCK);
                return(SYSERR);
        }
        tape58 . t_start = tcbuf . t_startc;
        tape58 . t_stop = tcbuf . t_stopc;
        tcbuf . t_startc = BCONT;
        tcbuf . t_stopc = BXOFF;
        if (ioctl(td, TIOCSETC, &tcbuf) < 0){
                terrno = errno - TEUNIX;
                (void) close(td);
                (void) unlink(TU58LOCK);
                return(SYSERR);
        }

        /* reset mode to raw and tandem */
        if (ioctl(td, TIOCGETP, &tsbuf) < 0){
                terrno = errno - TEUNIX;
                tcbuf . t_startc = tape58 . t_start;
                tcbuf . t_stopc = tape58 . t_stop;
                (void) ioctl(td, TIOCSETC, &tcbuf);
                (void) close(td);
                (void) unlink(TU58LOCK);
                return(SYSERR);
        }
        tape58 . t_sflag = tsbuf . sg_flags;
        tsbuf . sg_flags |= RAW | TANDEM;
        if (ioctl(td, TIOCSETP, &tsbuf) < 0){
                terrno = errno - TEUNIX;
                tcbuf . t_startc = tape58 . t_start;
                tcbuf . t_stopc = tape58 . t_stop;
                (void) ioctl(td, TIOCSETC, &tcbuf);
                (void) close(td);
                (void) unlink(TU58LOCK);
                return(SYSERR);
        }

        /* now call the reset routine; if successful, ready! */
        if ((x = treset(td)) != TENONE){
                terrno = x;
                tcbuf . t_startc = tape58 . t_start;
                tcbuf . t_stopc = tape58 . t_stop;
                (void) ioctl(td, TIOCSETC, &tcbuf);
                tsbuf . sg_flags = tape58 . t_sflag;
                (void) ioctl(td, TIOCSETP, &tsbuf);
                (void) close(td);
                (void) unlink(TU58LOCK);
                return(SYSERR);
        }

        /* return drive number */
        tape58 . t_open++;
        return(dn);
}

/*
 * tclose(dn)
 * int dn;                      drive number to be closed
 * this frees drive dn if it is open. then, if all drives are closed,
 * it closes the device after restoring the modes to what they were.
 *
 * RETURNS: -1 on error; terrno indicates what happened
 *          0  on success
 */
int tclose(dn)
int dn;                         /* drive being closed */
{
        int x;                  /* temp for error */
        struct dr58 *ds;        /* pointer to drive structure */
        struct sgttyb tsbuf;    /* temp for sgttyb block */
        struct tchars tcbuf;    /* temp for tchars block */

        /* check drive number */
        if (dn != 0 && dn != 1){
                terrno = TEBUNN;
                return(SYSERR);
        }

        /* see if it is closed */
        ds = &(tape58 . t_dr[dn]);
        if (tape58 . t_open == 0 || (ds -> d_flags & OPEN) == 0){
                terrno = TEICLO;
                return(SYSERR);
        }

        /* restore the tcb info */
        (void) ioctl(tape58 . t_fd, TIOCGETC, &tcbuf);
        tcbuf . t_startc = tape58 . t_start;
        tcbuf . t_stopc = tape58 . t_stop;
        (void) ioctl(tape58 . t_fd, TIOCSETC, &tcbuf);
        (void) ioctl(tape58 . t_fd, TIOCGETP, &tsbuf);
        tsbuf . sg_flags = tape58 . t_sflag;
        (void) ioctl(tape58 . t_fd, TIOCSETP, &tsbuf);

        /* close the drive and the unit if necessary */
        ds -> d_flags &= ~OPEN;
        tape58 . t_open -= 1;
        if ((x = tape58 . t_open) == 0)
                if ((x = close(tape58 . t_fd)) < 0 || (x = unlink(TU58LOCK)) < 0)
                        terrno = x - TEUNIX;
                else
                        x = 0;

        /* return status code */
        return(x == 0 ? 0 : SYSERR);
}

/*
 * tseek(dn, offset, whence)
 * int dn;                      drive to seek on
 * int offset;                  position to seek relative to whence
 * int whence;                  0 - seek from beginning, 1 - present,
 *                              2 - end, of tape
 *
 * this positions drive dn at the indicated block. the block number is
 * found with respect to special addressing mode. on error, an error return
 * is made; no recovery of any kind is attempted. on error the position
 * value may be corrupt. best to reset it in that case.
 *
 * RETURNS: -1 on error; terrno indicates what happened
 *          new position on success
 */
int tseek(dn, offset, whence)
int dn;                         /* drive number */
int offset;                     /* offset from whence location */
int whence;                     /* 0 - beginning, 1- current, 2 - end */
{
        int x;                  /* temp for error */
        int block;              /* block number */
        struct dr58 *ds;        /* pointer to drive structure */

        /* check drive number */
        if (dn != 0 && dn != 1){
                terrno = TEBUNN;
                return(SYSERR);
        }

        /* be sure drive is open */
        ds = &(tape58 . t_dr[dn]);
        if (tape58 . t_open == 0 || (ds -> d_flags & OPEN) == 0){
                terrno = TEDRNO;
                return(SYSERR);
        }

        /* get the block number */
        switch(whence){
        case 0:                         /* from beginning */
                block = offset;
                break;
        case 1:                         /* from current position */
                block = offset + ds -> d_posit;
                break;
        case 2:                         /* from end */
                if (ds -> d_flags & SAM)
                        block = NBLKSAM - 1;
                else
                        block = NBLKNORM - 1;
                block -= offset;
                break;
        default:                        /* bad arg to system call */
                terrno = EINVAL - TEUNIX;
                return(SYSERR);
        }

        /* Now position the tape */
        if ((x = tcack(tape58 . t_fd, CPOSIT, ds -> d_mods, dn, ds -> d_swit,
                                      0, block)) != TENONE){
                terrno = x;
                return(SYSERR);
        }

        /* return new block number */
        return(block);
}

/*
 * tread(dn, buf, nbytes)
 * int dn;                      drive to be read from
 * char *buf;                   buffer for incoming chars
 * int nbytes;                  number of bytes to be read
 *
 * this reads nbytes bytes into buf from drive dn.  NO CHECKSUM VERIFYING
 * IS DONE (yet), so watch it. on any error, you get an error return; no
 * recovery of any kind is attempted. reading begins at the current position
 * of the drive; its position is updated only if there is no error, and
 * on error the position value may be corrupt. best to reset it in that
 * case.
 *
 * RETURNS: -1 on error; terrno indicates what happened
 *          number of bytes read on success
 */
int tread(dn, buf, nbytes)
int dn;                         /* drive number */
char *buf;                      /* buffer */
int nbytes;                     /* number of bytes to be read */
{
        int nby;                /* temporary for number of bytes to read */
        int nbl;                /* number of blocks to read */
        int bytes;              /* size of next data packet */
        int b;                  /* counter in for loop */
        int x;                  /* temp for error */
        struct dr58 *ds;        /* pointer to drive structure */

        /* check drive number and drive */
        if (dn != 0 && dn != 1){
                terrno = TEBUNN;
                return(SYSERR);
        }

        /* be sure the drive is open */
        ds = &(tape58 . t_dr[dn]);
        if (tape58 . t_open == 0 || (ds -> d_flags & ROPEN) != ROPEN){
                terrno = TENORD;
                return(SYSERR);
        }

        /* send command packet */
        if ((x = tcnak(tape58 . t_fd, CREAD, ds -> d_mods, dn, ds -> d_swit,
                                        nbytes, ds -> d_posit)) != TENONE){
                terrno = x;
                return(SYSERR);
        }

        /* now read the data, IOBLKSZ bytes at a time */
        nby = nbytes;
        nbl = (nbytes + IOBLKSZ - 1) / IOBLKSZ;
        for (b = 0; b < nbl; b++){
                bytes = (nby > IOBLKSZ) ? IOBLKSZ : nby;
                if ((x = tgdata(tape58 . t_fd, buf + b * IOBLKSZ)) != TENONE){
                        terrno = x;
                        break;
                }
                nby -= bytes;
        }

        /* get end packet, check for errors */
        if ((x = tpchk(tape58 . t_fd)) != TENONE){
                terrno = x;
                return(SYSERR);
        }

        /* return number of bytes read */
        ds -> d_posit += delblk(ds -> d_mods & SAM);

        /* return number of bytes successfully read */
        return(abytes);
}

/*
 * twrite(dn, buf, nbytes)
 * int dn;                      drive to be written to
 * char *buf;                   buffer for outgoing chars
 * int nbytes;                  number of bytes to be written
 *
 * this writes the first nbytes in buf to dn. a checksum is computed and in
 * verify mode (VFY set in modifier byte) it is checked. any error gives
 * an error return; no recovery of any kind is attempted. writing begins
 * at the current position of the drive; its position is updated only if
 * there is no error, and on error the position value may be corrupt.
 * best to reset it in that case.
 *
 * RETURNS: -1 on error; terrno indicates what happened
 *          number of bytes written on success
 */
int twrite(dn, buf, nbytes)
int dn;                         /* drive number */
char *buf;                      /* buffer */
int nbytes;                     /* number of bytes to be written */
{
        int nby;                /* temporary for number of bytes to write */
        int nbl;                /* number of blocks to write */
        int bytes;              /* size of next data packet */
        int b;                  /* counter in for loop */
        int x;                  /* temp for error */
        struct dr58 *ds;        /* pointer to drive structure */

        /* check drive number and drive */
        if (dn != 0 && dn != 1){
                terrno = TEBUNN;
                return(SYSERR);
        }

        /* be sure the drive is open */
        ds = &(tape58 . t_dr[dn]);
        if (tape58 . t_open == 0 || (ds -> d_flags & WOPEN) != WOPEN){
                terrno = TENORD;
                return(SYSERR);
        }

        /* send command packet */
        if ((x = tcack(tape58 . t_fd, CWRITE, ds -> d_mods, dn, ds -> d_swit,
                                        nbytes, ds -> d_posit)) != TENONE){
                terrno = x;
                return(SYSERR);
        }

        /* now write the data, IOBLKSZ bytes at a time */
        nby = nbytes;
        nbl = (nbytes + IOBLKSZ - 1) / IOBLKSZ;
        for (b = 0; b < nbl; b++){
                bytes = (nby > IOBLKSZ) ? IOBLKSZ : nby;
                if ((x = tpdata(tape58 . t_fd, buf + b * IOBLKSZ, bytes)) != TENONE){
                        terrno = x;
                        break;
                }
                nby -= bytes;
        }

        /* return number of bytes written */
        ds -> d_posit += delblk(ds -> d_mods & SAM);

        /* return number of bytes successfully written */
        return(abytes);
}

/*
 * tioctl(dn, request, arg)
 * int dn;                      drive to be diddled
 * int request;                 how to diddle it
 * union tio *arg;              argument for diddling
 *
 * this changes or reports information stored in dn's control block.
 * information is returned through arg, which points to an integer or
 * a drive structure.
 *
 * RETURNS: -1 on error; terrno indicates what happened
 *          0 on success
 */
int tioctl(dn, request, arg)
int dn;                         /* drive number */
int request;                    /* what you want */
union tio *arg;                 /* where it goes */
{
        struct dr58 *ds;        /* pointer to drive structure */

        /* check drive number and drive */
        if (dn != 0 && dn != 1){
                terrno = TEBUNN;
                return(SYSERR);
        }

        /* be sure the drive is open */
        ds = &(tape58 . t_dr[dn]);
        if (tape58 . t_open == 0 || (ds -> d_flags & OPEN) == 0){
                terrno = TENORD;
                return(SYSERR);
        }

        /* process the request */
        switch(request){
        case TU58SSAM:          /* set special addressing mode */
                ds -> d_mods |= SAM;
                return(0);
        case TU58CSAM:          /* clear special addressing mode */
                ds -> d_mods &= ~SAM;
                return(0);
        case TU58SVFY:          /* set verify mode */
                ds -> d_mods |= VFY;
                return(0);
        case TU58CVFY:          /* clear verify mode */
                ds -> d_mods &= ~VFY;
                return(0);
        case TU58SMTM:          /* set maintenance mode */
                ds -> d_swit |= MTM;
                return(0);
        case TU58CMTM:          /* clear maintenance mode */
                ds -> d_swit &= ~MTM;
                return(0);
        case TU58GDCB:          /* gets drive control block */
                arg -> drive . d_flags = ds -> d_flags;
                arg -> drive . d_posit = ds -> d_posit;
                arg -> drive . d_mods = ds -> d_mods;
                arg -> drive . d_swit = ds -> d_swit;
                return(0);
        case TU58SPOS:          /* set new block position */
                ds -> d_posit = arg -> where;
                return(0);
        default:                /* bad value */
                terrno = EINVAL - TEUNIX;
                return(SYSERR);
        }

}

/*
 *=========================================================================
 * from here on, the routines are invisible to any other routines not in
 *                               this file
 *=========================================================================
 *
 * tcnak: makes and sends a command packet without acknowledgement
 */
static int tcnak(fd, cmd, mod, drive, swit, n, blkno)
int fd;                         /* TU58 file descriptor */
char cmd;                       /* TU58 command opcode byte */
char mod;                       /* TU58 command modifier byte */
int drive;                      /* TU58 drive being commanded */
char swit;                      /* TU58 command switches byte */
int n;                          /* number of bytes being used in this command */
int blkno;                      /* block number being used in this command */
{
        char cpkt[14];          /* command packet */
        int x;                  /* checksum */

        /* make the command packet */
        cpkt[0] = (char) BCTRL;         /* signal command packet */
        cpkt[1] = (char) 0xa;           /* number of bytes in this packet */
        cpkt[2] = cmd;                  /* command byte */
        cpkt[3] = mod;                  /* modifier byte */
        cpkt[4] = (char) drive;         /* drive number */
        cpkt[5] = swit;                 /* switch byte */
        cpkt[6] = cpkt[7] = '\0';       /* sequence number */
                                        /* number of data bytes */
        cpkt[8] = (char) (n & 0377);
        cpkt[9] = (char) ((n >> 8) & 0377);
                                        /* block number bytes */
        cpkt[10] = (char) (blkno & 0377);
        cpkt[11] = (char) ((blkno >> 8) & 0377);
                                        /* checksum */
        cpkt[12] = cpkt[13] = '\0';
        x = chksum(cpkt, 12);
        cpkt[12] = (char) (x & 0377);
        cpkt[13] = (char) ((x >> 8) & 0377);

        /* send the sucker */
        if (write(fd, cpkt, 14) != 14)
                return(errno - TEUNIX);

        /* return */
        return(TENONE);

}
/*
 * tcnak: makes and sends a command packet with acknowledgement
 */
static int tcack(fd, cmd, mod, drive, swit, n, blkno)
int fd;                         /* TU58 file descriptor */
char cmd;                       /* TU58 command opcode byte */
char mod;                       /* TU58 command modifier byte */
int drive;                      /* TU58 drive being commanded */
char swit;                      /* TU58 command switches byte */
int n;                          /* number of bytes being used in this command */
int blkno;                      /* block number being used in this command */
{
        int x;                  /* error code */

        /* send command packet */
        if ((x = tcnak(fd, cmd, mod, drive, swit, n, blkno)) != TENONE)
                return(x);

        /* return response */
        return(tpchk(fd));

}

/*
 * tpdata: puts a data packet onto the TU58
 */
static int tpdata(fd, loc, num)
int fd;                         /* TU58 file descriptor */
char *loc;                      /* beginning of the byte stream */
int num;                        /* number of bytes in the stream */
{
        char dpkt[132];         /* data packet */
        int i;                  /* counter in a for loop */
        int x;                  /* checksum */

        /* set up the data packet */
        dpkt[0] = BDATA;        /* flag packet as data */
        dpkt[1] = (char) num;   /* number of data bytes */
                                /* put in the data bytes */
        for(i = 2; i < num + 2; i++)
                dpkt[i] = loc[i - 2];
                                /* now the checksum */
        dpkt[num + 2] = dpkt[num + 3] = '\0';
        x = chksum(dpkt, num + 2);
        dpkt[num + 2] = (char) (x & 0377);
        dpkt[num + 3] = (char) ((x >> 8) & 0377);

        /* send the sucker */
        if (write(fd, dpkt, num + 4) != num + 4)
                return(errno - TEUNIX);

        /* return response */
        return(tpchk(fd));

}

/*
 * tgdata: gets a data packet from the TU58
 */
static int tgdata(fd, loc)
int fd;                         /* TU58 file descriptor */
char *loc;                      /* location of block to be read */
{
        char pkt[134];          /* input byte */
        char chk[2];            /* input checksum */
        int x;                  /* checksum */
        int i;

        /* get first byte, be sure it's data */
        if (read(fd, pkt, 1) != 1)
                return(errno - TEUNIX);
        if (*pkt == BEND){              /* end -- probable screwup, look */
                if ((i = tend(fd)) == TESWRT){
                        terrno = i;
                        i = TENONE;
                }
                return(i);
        }
        if (*pkt != BDATA)              /* garbage - resync line */
                return(treset(fd) ? TEREIN : TECLBD);

        /* grab the size of the data and then the data */
        if (read(fd, pkt + 1, 1) != 1)
                return(errno - TEUNIX);
        x = (int) pkt[1];
        if (x == -128)
                x = 128;
        for(i = 0; i < x; i++){
                if (read(fd, loc + i, 1) != 1)
                        return(errno - TEUNIX);
                pkt[i + 2] = loc[i];
        }

        /* verify  the checksum */
        for(i = 0; i < 2; i++)
                if (read(fd, chk + i, 1) != 1)
                        return(errno - TEUNIX);
        i = (int) ((unsigned short int) (((chk[1]&0377)<<8)|(chk[0]&0377)));
        if (i != chksum(pkt, x+2))
                terrno = TEBCSR;

        /* return success */
        return(TENONE);
}

/*
 * tend: processes an end packet
 */
static int tend(fd)
int fd;                         /* TU58 file descriptor */
{
        char pkt[14];           /* end packet */
        int i;                  /* counter in a for loop */

        /* load the end packet */
        pkt[0] = (char) BEND;
        for(i = 1; i <= 13; i++)
                if (read(fd, pkt + i, 1) != 1)
                        return(errno - TEUNIX);

        /* verify the checksum */
        i = (int) ((unsigned short int) (((pkt[13]&0377)<<8)|(pkt[12]&0377)));
        if (i != chksum(pkt, 12))
                terrno = TEBCSR;

        /* set number of bytes operated on, return error status */
        abytes = (int) ((unsigned short int) (((pkt[9]&0377)<<8)|(pkt[8]&0377)));
        return((int) pkt[3]);
}

/*
 * delblk: this changes the number of bytes read to something the VAX
 *         can handle (a necessary VAX hack, it and the TU58 have
 *         different size "integers") and returns the number of blocks
 *         processed.
 */
static int delblk(sam)
int sam;                /* nonzero if special addressing mode on */
{
        /* check for special addressing mode */
        if (sam > 0){
                /* here, note 128 = -128; sign extention kills us */
                if (abytes == -128)
                        abytes = 128;
                else if (abytes < 0)
                        abytes += SAMSIZE;
                /* return number of blocks processed */
                return(nblock(abytes, SAMSIZE));
        }

        /* not special addressing mode; neg means complement */
        if (abytes < 0)
                abytes += NORMSIZE;
                /* return number of blocks processed */
        return(nblock(abytes, NORMSIZE));
}

/*
 * chksum: this computes a one's complement checksum
 */
static int chksum(begin, num)
char *begin;                    /* begin computing check sum here */
int num;                        /* compute it for this many bytes */
{
        int i;                  /* counter in a for loop */
        unsigned short int losum;       /* new checksum */
        unsigned short int prsum;       /* previous checksum */

        /* initialize */
        prsum = losum = 0;

        /* compute the sum */
        for(i = 0; i < num; i += 2){
                losum += (unsigned short int) (((begin[i+1]&0377)<<8)|(begin[i]&0377));
                if (prsum > losum)
                        losum++;
                prsum = losum;
        }

        /* put it away */
        return((int) losum);
}

/*
 * tpchk: this handles a TU58 response to a command or data packet
 */
static int tpchk(fd)
int fd;                         /* file descriptor */
{
        char rec;               /* incoming byte */
        int i;                  /* return code from end packet */

        /* get byte */
        if (read(fd, &rec, 1) != 1)
                return(errno - TEUNIX);

        /* process it */
        switch(rec & 0177){
        case BCONT:                     /* continue byte */
                return(TENONE);
        case BEND:                      /* end packet */
                if ((i = tend(fd)) == TESWRT){
                        terrno = i;
                        i = TENONE;
                }
                return(i);
        default:                        /* anything else - resync line */
                return(treset(fd) ? TEREIN : TECLBD);
        }
}

/*
 * treset: this resyncs the line
 */
static int treset(fd)
int fd;                         /* file descriptor */
{
        char xmit[2];           /* chars to be sent to the TU58 */
        int i;                  /* counter in a for loop */
        int x;                  /* temp for error */
        long numb;              /* chars waiting to be read */

        /* set break for 2 chars */
        if (ioctl(fd, TIOCSBRK, NULL) < 0){
                return(errno - TEUNIX);
        }
        if (write(fd, xmit, 2) != 2){
                x = errno - TEUNIX;
                (void) ioctl(fd, TIOCCBRK, NULL);
                return(x - TEUNIX);
        }
        if (ioctl(fd, TIOCCBRK, NULL) < 0){
                return(errno - TEUNIX);
        }

        /* now write the two init chars */
        xmit[0] = xmit[1] = BINIT;
        if (write(fd, xmit, 2) != 2){
                return(errno - TEUNIX);
        }

        /* now gobble until continue or timeout */
        for(i = 0; i < MAXCHAR; i++)
                if (ioctl(fd, FIONREAD, &numb) < 0)
                        return(errno - TEUNIX);
                else if (numb == 0L)
                        continue;
                else if (read(fd, xmit, 1) != 1){
                        return(errno - TEUNIX);
                }
                else if (((*xmit)&0177) == BCONT)
                        break;

        /* no response or wrong response - error return */
        if (i == MAXCHAR)
                return(TEINTU);

        /* return success */
        return(TENONE);
}
