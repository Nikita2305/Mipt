#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

const int bufsize = 4096;
char buffer[4096 + 1];

int connect_to_host(char* hostname)
{
    struct sockaddr_in conn;
    conn.sin_family = AF_INET;
    conn.sin_port = htons(80);
    struct hostent* addr = gethostbyname(hostname);
    conn.sin_addr.s_addr =
        inet_addr(inet_ntoa(**((struct in_addr**)addr->h_addr_list)));
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sfd, (struct sockaddr*)&conn, sizeof(conn)) == -1) {
        perror("connect");
        return -1;
    }
    return sfd;
}

char* get_value_by_key(char* s, char* key)
{
    char* i1 = strstr(s, key) + 2 + strlen(key);
    char* i2 = strstr(i1, "\r\n");
    char* it = i1;
    char* arr = malloc((int)(i2 - i1) + 1);
    while (it != i2) {
        arr[it - i1] = *it;
        it++;
    }
    arr[i2 - i1] = '\0';
    return arr;
}

int main(int argc, char* argv[])
{
    int sfd = connect_to_host(argv[1]);
    if (sfd == -1) {
        return 1;
    }

    // sending get
    dprintf(sfd, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", argv[2], argv[1]);

    // reading answer
    int to_write = 0;
    for (int i = 0;; i++) {
        int size = read(sfd, buffer, bufsize);
        if (size <= 0) {
            break;
        }
        buffer[size] = '\0';
        char* st = buffer;
        if (i == 0) {
            st = strstr(buffer, "\r\n\r\n") + 4;
            char* cl = get_value_by_key(buffer, "Content-Length");
            to_write = atoi(cl);
            free(cl);
        }
        to_write -= strlen(st);
        printf("%s", st);
        if (to_write == 0) {
            break;
        }
    }
    close(sfd);
}
