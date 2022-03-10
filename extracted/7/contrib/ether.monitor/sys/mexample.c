/* mip.c - mip */


/*----------------------------------------------------------------
 * mip - print IP packet information
 *----------------------------------------------------------------
 */

mip(stdin, stdout, stderr, nargs, args)
int    stdin, stdout, stderr, nargs;
char   *args[];
{
	int arg;

	printf("MIP\n");
	for(arg=1 ; arg<nargs ; arg++){
		switch(args[arg][0]){
		case 'f':
			fprintf(stdout,"f\n");
			fprintf(stdout,"%d\n",args[arg][1]);
			args[arg] += 2;
			fprintf(stdout,"%d\n",*args[arg]);
			fprintf(stdout,"%d\n",args[arg]);
			break;
		case 't':
			args[arg] += 1;
			fprintf(stdout,"%d\n",args[arg]);
			break;
		default :
			fprintf(stderr,"Syntax error\n");
			break;
		}
	}
}
