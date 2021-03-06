# include "stdio.h"
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 200
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin ={stdin}, *yyout ={stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
	int skipping;
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
		{skipping = 1;}
break;
case 2:
		{skipping = 0;}
break;
case 3:
		{extern	int	brkcount;
			 if (! skipping)
				if (++brkcount == 2)
					return(0); /* end-of-file */
				else
					return(DEFBRK);
			}
break;
case 4:
		{if (! skipping) return(COLON);}
break;
case 5:
		{if (! skipping) return(OCTAL);}
break;
case 6:
	{if (! skipping) return(INTEGER);}
break;
case 7:
		{if (! skipping) return(IS);}
break;
case 8:
		{if (! skipping) return(ON);}
break;
case 9:
		;
break;
case 10:
		{if (! skipping) return(IINT);}
break;
case 11:
		{if (! skipping) return(IINT);}
break;
case 12:
		{if (! skipping) return(OINT);}
break;
case 13:
		{if (! skipping) return(OINT);}
break;
case 14:
		{if (! skipping) return(CSR);}
break;
case 15:
		{if (! skipping) return(CSR);}
break;
case 16:
		{if (! skipping) return(ICSR);}
break;
case 17:
		{if (! skipping) return(ICSR);}
break;
case 18:
		{if (! skipping) return(IVEC);}
break;
case 19:
		{if (! skipping) return(IVEC);}
break;
case 20:
		{if (! skipping) return(OVEC);}
break;
case 21:
		{if (! skipping) return(OVEC);}
break;
case 22:
		{if (! skipping) return(INIT);}
break;
case 23:
		{if (! skipping) return(INIT);}
break;
case 24:
		{if (! skipping) return(OPEN);}
break;
case 25:
		{if (! skipping) return(OPEN);}
break;
case 26:
		{if (! skipping) return(CLOSE);}
break;
case 27:
		{if (! skipping) return(CLOSE);}
break;
case 28:
		{if (! skipping) return(READ);}
break;
case 29:
		{if (! skipping) return(READ);}
break;
case 30:
		{if (! skipping) return(GETC);}
break;
case 31:
		{if (! skipping) return(GETC);}
break;
case 32:
		{if (! skipping) return(PUTC);}
break;
case 33:
		{if (! skipping) return(PUTC);}
break;
case 34:
		{if (! skipping) return(WRITE);}
break;
case 35:
		{if (! skipping) return(WRITE);}
break;
case 36:
		{if (! skipping) return(SEEK);}
break;
case 37:
		{if (! skipping) return(SEEK);}
break;
case 38:
		{if (! skipping) return(CNTL);}
break;
case 39:
		{if (! skipping) return(CNTL);}
break;
case 40:
		;
break;
case 41:
		{extern int linectr;
			 linectr++;
			}
break;
case 42:
		{if (! skipping) return(IDENT);}
break;
case 43:
		{if (! skipping) return(yytext[0]);}
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */
int yyvstop[] ={
0,

43,
0,

40,
43,
0,

41,
0,

3,
43,
0,

43,
0,

43,
0,

43,
0,

5,
43,
0,

43,
0,

4,
43,
0,

9,
43,
0,

42,
43,
0,

27,
42,
43,
0,

31,
42,
43,
0,

23,
42,
43,
0,

39,
42,
43,
0,

25,
42,
43,
0,

33,
42,
43,
0,

29,
42,
43,
0,

37,
42,
43,
0,

35,
42,
43,
0,

40,
0,

2,
0,

26,
0,

30,
0,

22,
0,

38,
0,

24,
0,

32,
0,

28,
0,

36,
0,

34,
0,

1,
0,

5,
0,

6,
0,

42,
0,

42,
0,

42,
0,

42,
0,

7,
42,
0,

42,
0,

42,
0,

8,
42,
0,

42,
0,

15,
42,
0,

42,
0,

42,
0,

42,
0,

42,
0,

42,
0,

14,
0,

17,
42,
0,

10,
42,
0,

19,
42,
0,

12,
42,
0,

21,
42,
0,

16,
0,

11,
0,

18,
0,

13,
0,

20,
0,
0};
# define YYTYPE char
struct yywork { YYTYPE verify, advance; } yycrank[] ={
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
0,0,	4,24,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
4,24,	0,0,	1,6,	0,0,	
0,0,	0,0,	0,0,	1,7,	
0,0,	0,0,	1,8,	9,35,	
1,9,	1,10,	1,11,	7,25,	
0,0,	0,0,	0,0,	2,6,	
0,0,	0,0,	0,0,	1,12,	
2,7,	0,0,	1,13,	2,8,	
0,0,	2,9,	1,14,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
2,12,	0,0,	0,0,	2,13,	
10,36,	10,36,	10,36,	10,36,	
10,36,	10,36,	10,36,	10,36,	
10,36,	10,36,	11,37,	11,37,	
11,37,	11,37,	11,37,	11,37,	
11,37,	11,37,	11,37,	11,37,	
1,15,	0,0,	0,0,	0,0,	
1,16,	0,0,	1,17,	0,0,	
17,40,	0,0,	0,0,	1,18,	
1,19,	1,20,	17,41,	1,21,	
1,22,	2,15,	43,56,	15,39,	
1,23,	2,16,	30,51,	2,17,	
17,42,	26,47,	19,44,	17,43,	
2,18,	2,19,	2,20,	19,45,	
2,21,	2,22,	8,26,	30,52,	
39,53,	2,23,	8,27,	19,46,	
8,28,	40,54,	41,55,	44,57,	
46,58,	8,29,	8,30,	8,31,	
47,59,	8,32,	8,33,	48,60,	
49,61,	50,62,	8,34,	14,38,	
14,38,	14,38,	14,38,	14,38,	
14,38,	14,38,	14,38,	14,38,	
14,38,	51,63,	52,64,	54,65,	
55,66,	56,67,	57,68,	58,69,	
14,38,	14,38,	14,38,	14,38,	
14,38,	14,38,	14,38,	14,38,	
14,38,	14,38,	14,38,	14,38,	
14,38,	14,38,	14,38,	14,38,	
14,38,	14,38,	14,38,	14,38,	
14,38,	14,38,	14,38,	14,38,	
14,38,	14,38,	60,70,	61,71,	
62,72,	63,73,	14,38,	64,74,	
14,38,	14,38,	14,38,	14,38,	
14,38,	14,38,	14,38,	14,38,	
14,38,	14,38,	14,38,	14,38,	
14,38,	14,38,	14,38,	14,38,	
14,38,	14,38,	14,38,	14,38,	
14,38,	14,38,	14,38,	14,38,	
14,38,	14,38,	28,48,	0,0,	
0,0,	0,0,	0,0,	0,0,	
28,49,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	28,50,	0,0,	0,0,	
0,0};
struct yysvf yysvec[] ={
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-18,	yysvec+1,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+4,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+6,
yycrank+0,	0,		yyvstop+8,
yycrank+4,	0,		yyvstop+11,
yycrank+35,	0,		yyvstop+13,
yycrank+5,	0,		yyvstop+15,
yycrank+32,	0,		yyvstop+17,
yycrank+42,	0,		yyvstop+20,
yycrank+0,	0,		yyvstop+22,
yycrank+0,	0,		yyvstop+25,
yycrank+107,	0,		yyvstop+28,
yycrank+4,	yysvec+14,	yyvstop+31,
yycrank+0,	yysvec+14,	yyvstop+35,
yycrank+9,	yysvec+14,	yyvstop+39,
yycrank+0,	yysvec+14,	yyvstop+43,
yycrank+21,	yysvec+14,	yyvstop+47,
yycrank+0,	yysvec+14,	yyvstop+51,
yycrank+0,	yysvec+14,	yyvstop+55,
yycrank+0,	yysvec+14,	yyvstop+59,
yycrank+0,	yysvec+14,	yyvstop+63,
yycrank+0,	yysvec+4,	yyvstop+67,
yycrank+0,	0,		yyvstop+69,
yycrank+10,	0,		yyvstop+71,
yycrank+0,	0,		yyvstop+73,
yycrank+131,	0,		yyvstop+75,
yycrank+0,	0,		yyvstop+77,
yycrank+17,	0,		yyvstop+79,
yycrank+0,	0,		yyvstop+81,
yycrank+0,	0,		yyvstop+83,
yycrank+0,	0,		yyvstop+85,
yycrank+0,	0,		yyvstop+87,
yycrank+0,	0,		yyvstop+89,
yycrank+0,	yysvec+10,	yyvstop+91,
yycrank+0,	yysvec+11,	yyvstop+93,
yycrank+0,	yysvec+14,	yyvstop+95,
yycrank+22,	yysvec+14,	yyvstop+97,
yycrank+26,	yysvec+14,	yyvstop+99,
yycrank+32,	yysvec+14,	yyvstop+101,
yycrank+0,	yysvec+14,	yyvstop+103,
yycrank+17,	yysvec+14,	yyvstop+106,
yycrank+33,	yysvec+14,	yyvstop+108,
yycrank+0,	yysvec+14,	yyvstop+110,
yycrank+43,	yysvec+14,	yyvstop+113,
yycrank+34,	0,		0,	
yycrank+36,	0,		0,	
yycrank+42,	0,		0,	
yycrank+52,	0,		0,	
yycrank+55,	0,		0,	
yycrank+65,	0,		0,	
yycrank+0,	yysvec+14,	yyvstop+115,
yycrank+53,	yysvec+14,	yyvstop+118,
yycrank+52,	yysvec+14,	yyvstop+120,
yycrank+70,	yysvec+14,	yyvstop+122,
yycrank+54,	yysvec+14,	yyvstop+124,
yycrank+72,	yysvec+14,	yyvstop+126,
yycrank+0,	0,		yyvstop+128,
yycrank+84,	0,		0,	
yycrank+83,	0,		0,	
yycrank+101,	0,		0,	
yycrank+85,	0,		0,	
yycrank+104,	0,		0,	
yycrank+0,	yysvec+14,	yyvstop+130,
yycrank+0,	yysvec+14,	yyvstop+133,
yycrank+0,	yysvec+14,	yyvstop+136,
yycrank+0,	yysvec+14,	yyvstop+139,
yycrank+0,	yysvec+14,	yyvstop+142,
yycrank+0,	0,		yyvstop+145,
yycrank+0,	0,		yyvstop+147,
yycrank+0,	0,		yyvstop+149,
yycrank+0,	0,		yyvstop+151,
yycrank+0,	0,		yyvstop+153,
0,	0,	0};
struct yywork *yytop = yycrank+249;
struct yysvf *yybgin = yysvec+1;
char yymatch[] ={
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
'0' ,'1' ,'1' ,'1' ,'1' ,'1' ,'1' ,'1' ,
'1' ,'1' ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,'A' ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,01  ,
0};
char yyextra[] ={
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank){		/* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	}
yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
