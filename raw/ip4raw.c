#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <string.h>
#include <unistd.h>

u_int16_t ip_sum_calc(u_int16_t len_ip_header, u_int16_t buff[]) {
  u_int16_t word16;
  u_int32_t sum=0;
  u_int16_t i;
    
  // make 16 bit words out of every two adjacent 8 bit words in the packet
  // and add them up
  for (i=0;i<len_ip_header;i=i+2){
    word16 =((buff[i]<<8)&0xFF00)+(buff[i+1]&0xFF);
    sum = sum + (u_int32_t) word16;	
  }
	
  // take only 16 bits out of the 32 bit sum and add up the carries
  while (sum>>16)
    sum = (sum & 0xFFFF)+(sum >> 16);

  // one's complement the result
  sum = ~sum;
	
  return ((u_int16_t) sum);
}

void print_ip(void *pkt) {
  int line = 0;
  struct iphdr *ip = pkt;
  char str_dst[INET_ADDRSTRLEN], str_src[INET_ADDRSTRLEN];

  inet_ntop(AF_INET, &ip->saddr, str_src, INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &ip->daddr, str_dst, INET_ADDRSTRLEN);

  printf("                        10                20                  30\n");   
  printf("     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 0 1 2 3 4 5 6 7 8 9 0 1 2\n"); 
  printf("    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
  printf("% 2d: | v:% 3d | l:% 3d | tos:% 9d | totlen:% 22d |\n", ++line, ip->version, ip->ihl, ip->tos, ip->tot_len);
  printf("% 2d: | id:% 26d | frag_off:% 20d |\n", ++line, ip->id, ip->frag_off);
  printf("% 2d: | ttl:% 9d | proto:% 7d | chksum:% 22x |\n", ++line, ip->ttl, ip->protocol, ip->check);
  printf("% 2d: | source:% 43s : %08x |\n", ++line, str_src, ip->saddr);
  printf("% 2d: | destination:% 38s : %08x |\n", ++line, str_dst, ip->daddr);

  while( line < ip->ihl) {
    printf("% 2d: |  |\n", ++line);
  }
  
  printf("    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
}

void print_icmp(void *pkt) {
  int line = 0;
  struct icmphdr *icmp = pkt;
  
  printf("                        10                20                  30\n");   
  printf("     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 0 1 2 3 4 5 6 7 8 9 0 1 2\n"); 
  printf("    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
  printf("% 2d: | type:% 8d | code:% 8d | chksum:% 23x|\n", ++line, icmp->type, icmp->code, icmp->checksum);

  switch(icmp->type) {
  case ICMP_ECHOREPLY:
    printf("ICMP_ECHOREPLY\n");
    break;
  case ICMP_ECHO:
    printf("ICMP_ECHO\n");
    break;
  default:
    break;
  } 

  printf("    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");

}

int main(int argc, char* argv[]) {
  
  int i;
  struct iphdr *ip;
  struct icmphdr *icmp;
  struct sockaddr_in conn;
  int sockfd;
  char on = 1;
  char off = 0;
  char *packet;
  char *src_addr, *dst_addr;

  src_addr = argv[1];
  dst_addr = argv[2];

  if (getuid() != 0)    {
    printf("%s: root privelidges needed\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if(argc != 3) {
    printf("%s: Need 2 commandline arguments. You provided %d.\n", argv[0], argc);
    exit(EXIT_FAILURE);
  }

  printf("From %s, To: %s\n", argv[1], argv[2]);

  packet = (char*) malloc( sizeof(struct iphdr)+ sizeof(struct icmphdr ));

  memset(&ip, 0, sizeof(ip));

  ip = (struct iphdr*) packet;
  ip->ihl			= 5;
  ip->version			= 4;
  ip->tos			= 0;
  ip->tot_len			= sizeof(struct iphdr) + sizeof(struct icmphdr);
  ip->id			= htons(random());
  ip->ttl			= 255;
  ip->protocol		        = IPPROTO_ICMP;
  inet_pton(AF_INET, argv[1], &ip->saddr);
  inet_pton(AF_INET, argv[2], &ip->daddr);
  //  ip->saddr			= inet_addr(argv[1]);
  //  ip->daddr			= inet_addr(argv[2]);
  ip->check = ip_sum_calc(5, (u_int16_t*)&ip);
  print_ip(ip);

  memset(&icmp, 0, sizeof(struct icmphdr));

  icmp = (struct icmphdr*) (packet + ip->ihl);
  icmp->type                    = ICMP_ECHO;
  icmp->code = 0;
  //  icmp->checksum = ip_sum_calc(sizeof(struct icmphdr), &icmp);
  icmp->un.echo.id = 0;
  icmp->un.echo.sequence = 0;

  print_icmp(icmp);

  conn.sin_family = AF_INET;
  inet_pton(AF_INET, argv[2], &conn.sin_addr.s_addr);

  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  perror("socket() : ");
  setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
  perror("setsockopt() : ");
  sendto(sockfd, packet, ip->tot_len, 0, (struct sockaddr *)&conn, sizeof(struct sockaddr));
  perror("sendto() : ");


}
