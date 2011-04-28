#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for memset
#include "gopt.h"

#define BUFLEN 2

int main(int argc, char* argv[])
{
	int i=0; /* holds the return (read bytes) from fread() */
	int nc=0; /* Sum of read bytes. */
	int loopc=0; /* number of loop iterations */
	char inbuf[BUFLEN+1]; /* Input-buffer */

	void *options= gopt_sort( & argc, argv, gopt_start(
					  gopt_option( 'h', 0, gopt_shorts( 'h', '?' ), gopt_longs( "help", "HELP" )),
					  gopt_option( 'l', GOPT_ARG, gopt_shorts( 'l' ), gopt_longs( "log" )),
					  gopt_option( 'o', GOPT_ARG, gopt_shorts( 'o' ), gopt_longs( "output" ))));

	if( gopt( options, 'h' ) ) {
		/*
		 * if any of the help options was specified
		 */
		fprintf( stdout, "HELP!\n" );
		exit( EXIT_SUCCESS );
	}
	
	/* Prepare (zero) the data-buffer */
	memset(inbuf, 0, BUFLEN+1);

	i = fread(inbuf, sizeof(char), BUFLEN, stdin);
	do {
		/* Output to stderr (or log) */
		loopc +=1;
		fprintf(stderr, "loopc: %d\n", loopc);
		nc += i;
		fprintf(stderr, "Read characters: %s\n", inbuf);
		/* Output to stdout */
		fwrite(inbuf, sizeof(char), i, stdout);
	}
	while (BUFLEN == (i = fread(inbuf, sizeof(char), BUFLEN, stdin)  ) ) ;

	fprintf(stderr, "Counted characters: %d\n", nc);

	return EXIT_SUCCESS;
}
