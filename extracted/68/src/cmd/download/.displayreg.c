/*
 *==============================================
 * displayreg - display a register on one line
 *==============================================
 */
displayreg(val)
int val;
{
	char buf[32];
        if ( val < 0 )
                strcpy(buf,"PS");
        else
                sprintf(buf,"R%d",val);
        strncat(buf,"        ",8-strlen(buf));
	fprintf(logfp, "\r%s",buf);
	fflush(stdout);
}


