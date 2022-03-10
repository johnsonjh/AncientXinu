# 1 "/mega/src/prom/srec/promsrec.c"







































# 1 "/mega/src/prom/srec/cons.h"




struct	consregs	{

	unsigned char filler1;
	unsigned char data;
	unsigned char filler2;
	unsigned char filler3;
	unsigned char filler4;
	unsigned char stat;
	unsigned char filler5;
	unsigned char filler6;
	unsigned char filler7;
	unsigned char mode;
	unsigned char filler8;
	unsigned char filler9;
	unsigned char filler10;
	unsigned char command;
	unsigned char filler11;
	unsigned char filler12;
};





struct consregs *consdev;





























































# 40 "/mega/src/prom/srec/promsrec.c"









char *errmsg[] = {
	"Bad hex number",
	"Bad S-record type",
	"Bad record length",
	"Record must start with S",
	"Bad checksum",
	"Bad hex nibble"
};










char getabyte(),getab();


char scount;
int addr;
char sdata[40];
char srec[80];






main()
{
	register char c;

	enable((struct consregs *)0xC100    );
	initcons();
	printf("Megatest Prom S-record loader. Version 1.1\n");
	
	for(;;) {
		
		gets(srec);
		
		sput();
	}
}


sput()
{
	register char i;
	char sumcnt;

printf("sput: %s\n",srec);
	if (*srec != 'S')
		error(	3);
	
	scount = getabyte(2);
	
	if (lenstr(srec)-scount*2-2-2)
		error(2);
	
	for (i=0;i<scount;i++)
		sdata[i] = getabyte(2+2+i*2);
	
	for (sumcnt=scount,i=0;i<scount-1;i++)
		sumcnt += sdata[i];
	if ((~sumcnt) != sdata[scount-1])
		error(	4);
	
	switch(srec[1]) {
	case '0':
	case '9':
		
		break;
	case '1':
		
		getaddr(2);
		loadit(2);
		break;
	case '2':
		
		getaddr(3);
		loadit(3);
		break;
	case '3':
		
		getaddr(3);
		bss();
		break;
	case '8':
		
		getaddr(3);
		execit();
		break;
	default:
		
		error(1);
		break;
	}
}


getaddr(len)
{
	register i;

	addr = 0;
	for(i=0;i<len;i++) {
		addr <<= 8;
		addr |= sdata[i] & 0xff;
	}
}


loadit(len)
{
	register char count;

printf("0x%x:\t",addr);
	for(count=len;count<scount-1;count++) {
		printf("%2x ",sdata[count]);
		*(char *)addr++ = sdata[count];
	}
printf("\n");
}	


bss()
{
	int len;
	register char i;

	
	len = 0;
	for(i=0;i<3;i++) {
		len <<= 8;
		len |= sdata[i] & 0xff;
	}
printf("Bss: %d bytes starting at %x\n",len,addr);
	while (len--)
		*(char *)addr++ = 0;
}


execit()
{
printf("Begin execution at %x\n",addr);
	asm("	movl	0x2000,sp");
	asm("	movl	_addr,a0");
	asm(	"	jmp		a0@");
	
}


char
getabyte(pos)
{
	return((getab(pos)<<4)|getab(pos+1));
}


char
getab(pos)
{
	char byte;

	byte = srec[pos];
	if (byte == ' ')
		return(0);
	if ((byte >= '0') && (byte <= '9'))
		return(byte-'0');
	if ((byte >= 'A') && (byte <='F'))
		return(byte-'A'+10);
	if ((byte >= 'a') && (byte <='f'))
		return(byte-'a'+10);
	error(5);
}


error(type)
{
	enable((struct consregs *)0xC100    );
	printf("Error\n");
	printf("%s\n",errmsg[type]);
}

lenstr(str)
char *str;
{
	register char i;

	i = 0;
	while (*str++) i++;
	return(i);
}
