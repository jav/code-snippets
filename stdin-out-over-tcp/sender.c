#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gopt.h"

#define BUFLEN 2

int main(int argc, const char **argv)
{
	int i=0; /* holds the return (read bytes) from fread() */
	int nc=0; /* Sum of read bytes. */
	int loopc=0; /* number of loop iterations */
	char inbuf[BUFLEN+1]; /* Input-buffer */
	const char *in_filename =0; 
	const char *out_filename =0; 
	const char *log_filename =0; 
	FILE *fout, *fin, *flog;

	void *options= gopt_sort( &argc, argv, gopt_start(
					  gopt_option( 'h', 0, gopt_shorts( 'h', '?' ), gopt_longs( "help", "HELP" )),
					  gopt_option( 'l', GOPT_ARG, gopt_shorts( 'l' ), gopt_longs( "log" )),
					  gopt_option( 'o', GOPT_ARG, gopt_shorts( 'o' ), gopt_longs( "out" )), 
					  gopt_option( 'i', GOPT_ARG, gopt_shorts( 'i' ), gopt_longs( "in" )) 
					  ));

	if( gopt( options, 'h' ) ) {
		/*
		 * if any of the help options was specified
		 */
		fprintf( stdout, "HELP!\n" );
		exit( EXIT_SUCCESS );
	}
	
        /* If we have specified an input file, or it is specified to "-" */
	if ( gopt_arg( options, 'i', &in_filename ) && strcmp( in_filename, "-" ) ) {
		fin = fopen( in_filename, "r" );
		if( ! fin ) {
			fprintf(stderr, "Could not open input (-i/--in) file.");
			exit( EXIT_FAILURE );
		}
	}
        /* If we have not specified an output file, or it is specified to "-" */
	else { 
		fin = stdin;
	}

        /* If we have specified an output file, or it is specified to "-" */
	if ( gopt_arg( options, 'o', &out_filename ) && strcmp( out_filename, "-" ) ) {
		fout = fopen( out_filename, "wb" );
		if( ! fout ) {
			fprintf(stderr, "Could not open output (-o/--out) file.");
			exit( EXIT_FAILURE );
		}
	}
        /* If we have not specified an output file, or it is specified to "-" */
	else { 
		fout = stdout;
	}

        /* If we have specified an log/error-output file, or it is specified to "-" */
	if ( gopt_arg( options, 'l', &log_filename ) && strcmp( log_filename, "-" ) ) {
		flog = fopen( log_filename, "wb" );
		if( ! flog ) {
			fprintf(stderr, "Could not open log/err-output (-l/--log) file.");
			exit( EXIT_FAILURE );
		}
	}
        /* If we have not specified an log/error-output file, or it is specified to "-" */
	else { 
		flog = stderr;
	}
	
	/* Prepare (zero) the data-buffer */
	memset(inbuf, 0, BUFLEN+1);

	i = fread(inbuf, sizeof(char), BUFLEN, fin);
	do {
		/* Output to stderr (or log) */
		loopc +=1;
		fprintf(flog, "loopc: %d\n", loopc);
		nc += i;
		fprintf(flog, "Read characters: i:%d, %s\n", i, inbuf);
		/* Output to stdout */
		fwrite(inbuf, sizeof(char), i, fout);
	}
	while (BUFLEN == (i = fread(inbuf, sizeof(char), BUFLEN, fin)  ) ) ;

	fprintf(flog, "Counted characters: %d\n", nc);

	return EXIT_SUCCESS;
}
