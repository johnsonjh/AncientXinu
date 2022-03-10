/*
 *============================================
 * displayval - display a number on one line
 *============================================
 */
displayval(val)
int val;
{
	char buf[32];
	sprintf(buf,"%d",val);
	strncat(buf,"        ",8-strlen(buf));
	fprintf(logfp, "\r%s",buf);
	fflush(stdout);
}
