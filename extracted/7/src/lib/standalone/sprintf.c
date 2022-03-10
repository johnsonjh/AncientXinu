/* sprintf.c - sprintf */

/*------------------------------------------------------------------------
 *  sprintf  --  format arguments and place output in a string
 *------------------------------------------------------------------------
 */
char	*sprintf(str, fmt, args)
        char *str;
        char *fmt;
{
        int     sprntf();
        char    *s;

        s = str;
        doprnt(fmt, &args, sprntf, &s);
        *s++ = '\0';
        return(str);
}

/*------------------------------------------------------------------------
 *  sprntf  --  routine called by doprnt to handle each character
 *------------------------------------------------------------------------
 */
static	sprntf(cpp, c)
        char    **cpp;
        char    c;
{
        return(*(*cpp)++ = c);
}
