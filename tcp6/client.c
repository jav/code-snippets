#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <resolv.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in6 serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin6_family = AF_INET6;

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    // Is it a 'raw' IPv6 address?
    if (inet_pton(AF_INET6, argv[1], &serv_addr.sin6_addr) > 0) {
	    
	    //note inet_pton will take care of setting the address
	    
	    serv_addr.sin6_family = AF_INET6;

    }
    else {
    
	    //res init is defined in resolv.h 
	    res_init();
	    _res.options |= RES_USE_INET6;
	    server = gethostbyname(argv[1]);
	    bcopy((char *)server->h_addr, 
		  (char *)&serv_addr.sin6_addr.s6_addr,
		  server->h_length);
    }

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    
    serv_addr.sin6_port = htons(portno);
    if (connect(sockfd,&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    return 0;
}
