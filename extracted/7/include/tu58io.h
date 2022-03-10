/*
 * structure of the TU58 descriptor block
 */
#ifndef NDRIVES
#define NDRIVES         2               /* number of drives on the TU58 */
#endif

struct dr58 {
        int     d_flags;        /* READ | WRITE | OPEN */
        int     d_posit;        /* current block number */
        char    d_mods;         /* SAM | VFY */
        int     d_swit;         /* MTM */
};

struct tu58 {
        int             t_open;         /* 1 if TU58 is open */
        int             t_fd;           /* file descriptor of TU58 */
        short           t_sflag;        /* original status flags (in sgttyb) */
        char            t_start;        /* original start char (in tchars) */
        char            t_stop;         /* original stop char (in tchars) */
        struct dr58     t_dr[NDRIVES];  /* drive structures */
};

/*
 * union for tioctl call
 */
union tio {
        struct dr58     drive;          /* drive controller */
        int             where;          /* drive position */
};

/*
 * tioctl defines
 */
#define TU58SSAM        1               /* set special addressing mode */
#define TU58CSAM        2               /* clear special addressing mode */
#define TU58SVFY        3               /* set verify mode */
#define TU58CVFY        4               /* clear verify mode */
#define TU58SMTM        5               /* set maintenance mode */
#define TU58CMTM        6               /* clear maintenance mode */
#define TU58GDCB        7               /* gets drive control block */
#define TU58SPOS        8               /* set new block position */
