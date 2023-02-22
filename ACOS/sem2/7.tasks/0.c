#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {    
    uint16_t port = atoi(argv[1]);
    
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(0); 

    int value;
    const int bufsize = 64;
    char buffer[bufsize];
    while (scanf("%d", &value) != EOF) {
        if (0 == sendto(sfd, &value, sizeof(int), 0, (const struct sockaddr*)&addr, sizeof(struct sockaddr_in))) {
            perror("sendto");
            return 1;
        }
        int recvd = 0;
        if (0 == (recvd = recvfrom(sfd, &value, sizeof(int), MSG_WAITALL, NULL, NULL))) {
            perror("recvfrom");
            return 1;
        }
        printf("%d\n", value);
    }
    close(sfd);
}
