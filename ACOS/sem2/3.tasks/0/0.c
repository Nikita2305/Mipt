#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main(int argv, char** argc) {
    uint16_t port = atoi(argc[2]);
    uint32_t ip = inet_addr(argc[1]);
   
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = ip;

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    
    int res = connect(sfd, (struct sockaddr*) &addr, sizeof(addr));
    if (res < 0) {
        printf("%s\n", "Error connecting");
        return 1;
    }

    int value;
    while (EOF != scanf("%d", &value)) {
        int network = htonl(value);
        unsigned char bytes[4];
        bytes[0] = (network >> 24) & 0xFF;
        bytes[1] = (network >> 16) & 0xFF;
        bytes[2] = (network >> 8) & 0xFF;
        bytes[3] = (network >> 0) & 0xFF;
        int res = write(sfd, bytes, 4); 
        if (res <= 0) {
            // printf("ERROR1!\n");
            break;
        }
        char buffer[128];
        int just_read = read(sfd, buffer, 127);
        // printf("bytes: %d\n", just_read);
        if (just_read <= 0) {
            // printf("ERROR2!\n");
            break;
        }
        network = 0;
        network += buffer[0] << 24;
        network += buffer[1] << 16;
        network += buffer[2] << 8;
        network += buffer[3] << 0;
        network = ntohl(network);
        printf("%d\n", network);
    }

    shutdown(sfd, SHUT_RDWR);

    close(sfd);

}
