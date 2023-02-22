#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

struct send_packet {
  struct icmphdr header;
  char empty_space[56];
} __attribute__((__packed__));

unsigned int checksum(void *addr, int count) {
  unsigned short *saddr = addr;
  unsigned int sum = 0;
  while (count > 1) {
    sum += *saddr++;
    count -= 2;
  }
  if (count > 0) {
    sum += *(unsigned char *)saddr;
  }
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  return ~sum;
}

int main(int argc, char **argv) {
  struct timespec start;
  clock_gettime(CLOCK_MONOTONIC, &start);

  char *straddr = argv[1];
  int timeout_time = atoi(argv[2]);
  int interval_time = atoi(argv[3]);

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(0);
  addr.sin_addr.s_addr = inet_addr(straddr);

  int sfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sfd == -1) {
    return 1;
  }

  int packages_sent = 0;
  int packages_got = 0;
  while (1) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    if ((now.tv_nsec - start.tv_nsec) / 1000.0 +
            (now.tv_sec - start.tv_sec) * 1000000.0 >
        timeout_time * 1000000.0) {
      break;
    }

    usleep(interval_time);

    struct send_packet packet;
    memset(&packet, 0, sizeof(struct send_packet));
    packet.header.code = 0;
    packet.header.type = ICMP_ECHO;
    packet.header.un.echo.id = getpid();
    packet.header.un.echo.sequence = packages_sent++;
    packet.header.checksum = checksum(&packet, sizeof(struct send_packet));

    socklen_t addr_len = sizeof(struct sockaddr_in);
    if (-1 == sendto(sfd, &packet, sizeof(struct send_packet), 0,
                     (struct sockaddr *)&addr, addr_len)) {
      return 1;
    }

    if (-1 == recvfrom(sfd, &packet, sizeof(struct send_packet), 0,
                       (struct sockaddr *)&addr, &addr_len)) {
      return 1;
    }

    packages_got += 1;
  }

  printf("%d", packages_got);
}
