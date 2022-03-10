
/*
 *====================================================
 * lock - prevent simultaneous access to LSI-11 line
 *====================================================
 */
lock()
{
        int lf, len;
        char buf[100], name[100];

        /*
         * if lock file exists - someone is using the line (maybe me)
         */
        if ((lf=creat(LOCKFILE,0444)) < 0 ) {
                if ((lf=open(LOCKFILE,0)) < 0 ) {
                        perror(LOCKFILE);
                        exit(1);
                }
                buf[read(lf,buf,100)] = '\0';
                sscanf(buf,"%s",name);
                if ( strcmp(name,getlogin()) == 0 )
                        return;
                fprintf(stderr,"LSI in use by %s\n",buf);
                exit(1);
        }
        /*
         * not in use - reserve and start timer
         */
        sprintf(buf,"%s on %s",getlogin(), ttyname());
        write(lf,buf,strlen(buf));
        close(lf);
        if ( fork() == 0 ) {
                signal(SIGHUP, SIG_IGN);
                signal(SIGINT, SIG_IGN);
                signal(SIGTERM, SIG_IGN);
                signal(SIGQUIT, SIG_IGN);
                sprintf(buf,"LSI in use by %s on %s for",getlogin(),ttyname());
                execl(RESPGM,buf,"10","minutes");
                perror(RESPGM);
                exit(1);
        }
}
