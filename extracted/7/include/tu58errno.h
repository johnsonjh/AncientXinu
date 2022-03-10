/*
 * this contains a list of TU58 error mnemonics
 */
#define TENONE  0               /* no error */
                        /* errors -1 to -64 are defined by the TU58 */
#define TESWRT  -1              /* success but with retries */
#define TEFAST  -1              /* failed self-test */
#define TEEOME  -2              /* partial operation (end of medium) */
#define TEBUNN  -8              /* bad unit number */
#define TENCRT  -9              /* no cartridge */
#define TEWRPR  -11             /* write protected */
#define TEDCKE  -17             /* data check error */
#define TEBLNF  -32             /* seek error (block not found) */
#define TEMSTP  -33             /* motor stopped */
#define TEBOPC  -48             /* bad op code */
#define TEBBLN  -55             /* bad block number */
                        /* errors -65 to -128 result from */
                        /* UNIX/TU58 interactions         */
#define TEINTU  -65             /* TU58 did not respond to initialization */
#define TEDRNO  -66             /* drive not open */
#define TENORD  -67             /* drive not open for reading */
#define TENOWR  -68             /* drive not open for writing */
#define TEICLO  -69             /* tried to close a closed drive */
#define TEREOP  -70             /* tried to open an open drive */
#define TEREIN  -71             /* TU58 was re-initialized */
#define TECLBD  -72             /* TU58 commumication link broke down */
#define TEBCSR  -73             /* bad check sum on read */
#define TETULU  -74             /* tape unit locked up */
#define TELFNU  -75             /* unable to unlink lock file */
                        /* errors under -129 are UNIX errors */
#define TEUNIX  192             /* Add to error number to get UNIX error */

/*
 * macro returning TENONE on success, error on failure
 */
#define tuokay(x)       (((x)==TESWRT)?TENONE:(x))
