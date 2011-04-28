#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "gopt.h"

#define BUFLEN 10

int main(int argc, const char **argv)
{
	int i=0; /* holds the return (read bytes) from fread() */
	int nc=0; /* Sum of read bytes. */
	int loopc=0; /* number of loop iterations */
	char inbuf[BUFLEN+1]; /* Input-buffer */
	const char *out_filename =0; 
	const char *str_address, *str_port, *str_address_family;
	FILE *fout;
	int af=0;
	int fd_sock, fd_in;
	socklen_t sa_len;
	struct sockaddr *sa;
	struct sockaddr_in ip4_sa;


	void *options= gopt_sort( &argc, argv, gopt_start(
					  gopt_option( 'h', 0, gopt_shorts( 'h', '?' ), gopt_longs( "help", "HELP" )),
					  gopt_option( 'o', GOPT_ARG, gopt_shorts( 'o' ), gopt_longs( "out" )), 
					  gopt_option( 'a', GOPT_ARG, gopt_shorts( 0 ), gopt_longs( "address" )),
					  gopt_option( 't', GOPT_ARG, gopt_shorts( 0 ), gopt_longs( "address_family", "af" )),
					  gopt_option( 'p', GOPT_ARG, gopt_shorts( 'p' ), gopt_longs( "port" )) 
					  ));

	if( gopt( options, 'h' ) ) {
		/*
		 * if any of the help options was specified
		 */
		fprintf( stdout, "Usage: sender  \n" );
		fprintf( stdout, "              [-o file| --out file] \n" );
		fprintf( stdout, "\n" );
		fprintf( stdout, "Report bugs to <javier at ubillos.org>.\n" );
		exit( EXIT_SUCCESS );
	}
	
        /* If we have specified an output file, or it is specified to "-" */
	
	if( !gopt_arg( options, 'p', &str_port ) || 
	    !gopt_arg( options, 't', &str_address_family) ) {
		fprintf(stderr, "To use \"network\" as an output, --port and --af need to be specified\n");
		exit( EXIT_FAILURE );	
	}

	str_address = 0;
	gopt_arg( options, 'a', &str_address );
	
	if( 0 == strcmp("AF_INET", str_address_family) ) {

		ip4_sa.sin_family = af = AF_INET;
		memset(&ip4_sa, 0, sizeof(ip4_sa));
		sa = (struct sockaddr *)&ip4_sa;
		
		ip4_sa.sin_port = htons(atoi(str_port));
		
		if( gopt_arg( options, 'a', &str_address) ) {
			fprintf(stderr, "ERROR binding to specific address not yet supported.\n");
			exit( EXIT_FAILURE );
		}
		else {
			ip4_sa.sin_addr.s_addr = INADDR_ANY;
		}	  

		if( 0 > (fd_sock = socket(AF_INET, SOCK_STREAM, 0)) ) {
			perror("ERROR opening socket\n");
			exit( EXIT_FAILURE );
		}
		ip4_sa.sin_port = htons(atoi(str_port));

		fprintf(stderr, "bind()\n");
		if( 0 > bind(fd_sock, (struct sockaddr *)&ip4_sa, sizeof(ip4_sa)) ) {
			perror("ERROR could not bind\n");
			exit( EXIT_FAILURE );	
		}
		fprintf(stderr, "listen()\n");
		if( 0 > listen(fd_sock,5) ) {
			perror("ERROR could not listen\n");
			exit( EXIT_FAILURE );	
		}


	}
	else {
		fprintf(stderr, "Address family: %s is not a supported address family\n", str_address_family);
		exit( EXIT_FAILURE );	
	}

	if ( gopt_arg( options, 'o', &out_filename ) && strcmp( out_filename, "-" ) ) {
		fout = fopen( out_filename, "wb" );
		
		if( ! fout ) {
			fprintf(stderr, "Could not open output (-o/--out) file.\n");
			exit( EXIT_FAILURE );
		}
	}
        /* If we have not specified an output file, or it is specified to "-" */
	else { 
		fout = stdout;
	}

	/* Prepare (zero) the data-buffer */
	memset(inbuf, 0, BUFLEN+1);


	while(1) {
		sa_len = sizeof(ip4_sa);
		fprintf(stderr, "accept()\n");
		if ( 0 > (fd_in = accept(fd_sock, (struct sockaddr *)&ip4_sa, &sa_len)) )  {
			perror( "ERROR accept failed\n");
			exit( EXIT_FAILURE );
		}
		fprintf(stderr, "read()\n");
		while( 0 < (i = read(fd_in, inbuf, BUFLEN )) ) {
			fprintf(stderr, "loopc: %d\n", loopc);
			nc += i;
			fprintf(stderr, "Read characters: i:%d, %s\n", i, inbuf);
			/* Output to stdout */
			fwrite(inbuf, sizeof(char), i, fout);
		}
	}


	fprintf(stderr, "Counted characters: %d\n", nc);

	return EXIT_SUCCESS;
}
