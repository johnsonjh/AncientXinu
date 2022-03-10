#include <kernel.h>
#include <conf.h>
#include <necuart.h>

main()
{
	char	buffer[128];

	printf("This is it!\n");
	fprintf(OTHER, "senddown initialize.c\n");
	while(1) {
		read(OTHER, buffer, 10);
		write(CONSOLE, buffer, 10);
	}
	tdump();
}	
