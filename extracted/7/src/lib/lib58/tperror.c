/* LINTLIBRARY */
#include <errno.h>
#include <tu58errno.h>

static struct terrs {
        int ernum;              /* error number */
        char *ermsg;            /* error string */
} tfubar[] = {
        TENONE, ": no error\n",
                        /* TU58 errors */
        TESWRT, ": success but with retries\n",
        TEEOME, ": partial operation (end of medium)\n",
        TEBUNN, ": bad unit number\n",
        TENCRT, ": no cartridge\n",
        TEWRPR, ": write protected\n",
        TEDCKE, ": data check error\n",
        TEBLNF, ": seek error (block not found)\n",
        TEMSTP, ": motor stopped\n",
        TEBOPC, ": bad op code\n",
        TEBBLN, ": bad block number\n",
                        /* UNIX/TU58 interaction errors */
        TEINTU, ": TU58 did not respond to initialization\n",
        TEDRNO, ": drive not open\n",
        TENORD, ": drive not open for reading\n",
        TENOWR, ": drive not open for writing\n",
        TEICLO, ": tried to close a closed drive\n",
        TEREOP, ": tried to open an open drive\n",
        TEREIN, ": TU58 was re-initialized\n",
        TECLBD, ": TU58 commumication link broke down\n",
        TEBCSR, ": bad check sum on read\n",
        TETULU, ": tape unit locked up\n",
        TELFNU, ": unable to unlink lock file\n",
        0     , 0,
};

char *oops = ": unknown error\n";
extern int terrno;

tperror(s)
char *s;
{
        struct terrs *p;        /* counter in a for loop */

        /* is it a UNIX error */
        if (terrno < -129){
                perror(s);
                return;
        }

        /* no - linear search on the array */
        for(p = tfubar; p -> ermsg != (char *) 0; p++)
                if (terrno == p -> ernum){
                        (void) write(2, s, strlen(s));
                        (void) write(2, p -> ermsg, strlen(p -> ermsg));
                        return;
                }

        /* unknown message */
        (void) write(2, s, strlen(s));
        (void) write(2, oops, strlen(oops));
        return;
}
