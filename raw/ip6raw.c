#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h> // required by ip6.h
#include <netinet/in.h> // required by ip6.h
#include <netinet/ip6.h>
#include <netinet/icmp6.h> // For nxt hdr (neighbor discovery)

void print_128_bits(void* data)
{

  int i;
  for (i = 0; i < 15; i++)
    printf("%.2X:", ((u_char*)data)[i]);
  printf("%.2X\n", ((u_char*)data)[i]);

}

void print_ip_header(struct ip6_hdr *ip)
{
  int i;

  u_char src[INET6_ADDRSTRLEN], dst[INET6_ADDRSTRLEN];
  u_char ip_type[15];

  memset(src, 0, INET6_ADDRSTRLEN);
  memset(dst, 0, INET6_ADDRSTRLEN);

  inet_ntop(AF_INET6, &ip->ip6_src, src, sizeof(src));
  inet_ntop(AF_INET6, &ip->ip6_dst, dst, sizeof(dst));

  switch(ip->ip6_nxt) {
  case IPPROTO_IPV6:
    sprintf(ip_type, "IPPROTO_IPV6"); break;
  case IPPROTO_ICMPV6:
    sprintf(ip_type, "IPPROTO_ICMPV6"); break;
  case ND_NEIGHBOR_SOLICIT:
    sprintf(ip_type, "NEIGH_SOL"); break;
  case ND_NEIGHBOR_ADVERT:
    sprintf(ip_type, "NEIGH_ADV"); break;
  case ND_REDIRECT:
    sprintf(ip_type, "ND_REDIRE"); break;
  case IPPROTO_TCP:
    sprintf(ip_type, "IPPROTO_TCP"); break;
  case IPPROTO_UDP:
    sprintf(ip_type, "IPPROTO_UDP"); break;
  default:
    sprintf(ip_type, "%15d", ip->ip6_nxt);
  }

  printf("IP -+-+-+- p:%8p +-+- size: %8d +-+-+-+-+-+-+-+-+-+-+\n", ip, ip->ip6_plen);
  printf("|    %07X |     %010X | flowlabel:                 %08X |\n", ip->ip6_vfc, ip->ip6_vfc,  ip->ip6_flow);
  printf("| len:         %16d | %16s |    % 11hd |\n", ip->ip6_plen, ip_type, ip->ip6_hlim);
  printf("| src:            %45s     |\n", src);
  printf("| dst:            %45s     |\n", dst);
  printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");


}


int main( int argc, char* argv[]){

  int i, nbytes;
  int fd;
  struct ip6_hdr ip;
  struct iovec ip_vec[3];
  // usage


  memset(&ip, 0, sizeof(struct ip6_hdr));

  printf("IPs: &s:%p, IPd: %s:&p\n", argv[1], argv[2], argv[3], argv[4]);

  print_ip_header(&ip);

  // Set IP header
  ip.ip6_vfc = 0x60;        // version and tclass
  ip.ip6_flow = 0x0;        // Flowlabel
  ip.ip6_plen = sizeof(ip); // Payload length
  ip.ip6_nxt = 0x4;            // Next header IP in IP
  ip.ip6_hlim = (uint8_t)0x255;      // hop limit
  inet_pton(AF_INET6, argv[1], &ip.ip6_src);
  inet_pton(AF_INET6, argv[3], &ip.ip6_dst);
  
  print_ip_header(&ip);

  // Assemble 
  ip_vec[0].iov_base = &ip;
  ip_vec[0].iov_len = sizeof(ip);
  
  // init socket
  fd = 	socket(AF_INET6, SOCK_RAW, IPPROTO_RAW);


  
  if( 0 == (nbytes = writev(fd, ip_vec, 1)) )
      perror("writeev: ");
  else
    perror("writeev!:");
  

}
