#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "gopt.h"

#define BUFLEN 2

int main(int argc, const char **argv)
{
	int i=0; /* holds the return (read bytes) from fread() */
	int nc=0; /* Sum of read bytes. */
	int loopc=0; /* number of loop iterations */
	char inbuf[BUFLEN+1]; /* Input-buffer */
	const char *in_filename =0; 
	const char *str_address, *str_port, *str_address_family;
	char str_ip4_addr[INET_ADDRSTRLEN];
	FILE *fin;
	int fd_out;
	int af=0;
	struct sockaddr *sa;
	struct sockaddr_in ip4_sa;


	void *options= gopt_sort( &argc, argv, gopt_start(
					  gopt_option( 'h', 0, gopt_shorts( 'h', '?' ), gopt_longs( "help", "HELP" )),
					  gopt_option( 'i', GOPT_ARG, gopt_shorts( 'i' ), gopt_longs( "in" )), 
					  gopt_option( 'd', GOPT_ARG, gopt_shorts( 0 ), gopt_longs( "dest", "destination", "address" )),
					  gopt_option( 't', GOPT_ARG, gopt_shorts( 0 ), gopt_longs( "address_family", "af" )),
					  gopt_option( 'p', GOPT_ARG, gopt_shorts( 'p' ), gopt_longs( "port" )) 
					  ));

	if( gopt( options, 'h' ) ) {
		/*
		 * if any of the help options was specified
		 */
		fprintf( stdout, "Usage: sender [-i file| --in file] \n" );
		fprintf( stdout, "              outputs to stdout\n" );
		fprintf( stdout, "Report bugs to <javier at ubillos.org>.\n" );
		exit( EXIT_SUCCESS );
	}
	
        /* If we have specified an input file, or it is specified to "-" */
	if ( gopt_arg( options, 'i', &in_filename ) && strcmp( in_filename, "-" ) ) {
		fin = fopen( in_filename, "r" );
		if( ! fin ) {
			fprintf(stderr, "Could not open input (-i/--in) file.\n");
			exit( EXIT_FAILURE );
		}
	}
        /* If we have not specified an output file, or it is specified to "-" */
	else { 
		fin = stdin;
	}

/* OUTPUT 
 *     and -n is specified -> network
 *         check that we have set -f, -a, -p
 *         if( -f AF_INET && -a && -p) -> IPv4
 * else, -o/-out is not specified
 *     set stdout as fd_out
 */
        /* If we have specified an output file, or it is specified to "-" */
	if( !gopt_arg( options, 'd', &str_address ) || 
	    !gopt_arg( options, 'p', &str_port ) || 
	    !gopt_arg( options, 't', &str_address_family) ) {
		fprintf(stderr, "To use \"network\" as an output, --dest, --port and --af need to be specified\n");
		exit( EXIT_FAILURE );	
	}
	/* OUT-INET */
	
	
	if( 0 == strcmp("AF_INET", str_address_family) ) {
		ip4_sa.sin_family = af = AF_INET;
		memset(&ip4_sa, 0, sizeof(ip4_sa));
		sa = (struct sockaddr *) &ip4_sa;
		printf("str_address: %s\n", str_address);
		inet_pton(af, str_address, &(ip4_sa.sin_addr));
		inet_ntop(af, &(ip4_sa.sin_addr), str_ip4_addr, INET_ADDRSTRLEN);
		
		ip4_sa.sin_port = htons(atoi(str_port));
		printf("Connecting to %s : %s\n", str_ip4_addr, str_port);
		if( 0 > (fd_out = socket(AF_INET, SOCK_STREAM, 0)) ) {
			perror("ERROR opening socket\n");
			exit( EXIT_FAILURE );
		}
		
		printf("Socket opened, connecting to %s : %s\n", str_address, str_port);
		if( 0 > connect(fd_out, (struct sockaddr *)&ip4_sa, sizeof(ip4_sa) ) ) {
			perror("ERROR connecting socket\n");
			exit( EXIT_FAILURE );
		}
		printf("Socket connected\n");
	}
	else {
		fprintf(stderr, "Address family: %s is not a supported address family\n", str_address_family);
		exit( EXIT_FAILURE );	
	}
	
	
	/* Prepare (zero) the data-buffer */
	memset(inbuf, 0, BUFLEN+1);

	i = fread(inbuf, sizeof(char), BUFLEN, fin);
	do {
		/* Output to stderr (or log) */
		loopc +=1;
		fprintf(stderr, "loopc: %d\n", loopc);
		nc += i;
		fprintf(stderr, "Read characters: i:%d, %s\n", i, inbuf);
		/* Output to stdout */
		write(fd_out, inbuf, i);
	}
	while (BUFLEN == (i = fread(inbuf, sizeof(char), BUFLEN, fin)  ) ) ;

	fprintf(stderr, "Counted characters: %d\n", nc);

	return EXIT_SUCCESS;
}
