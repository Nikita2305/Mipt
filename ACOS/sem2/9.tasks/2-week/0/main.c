#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void SendGetReq(int sock, char* host, char* file)
{
    dprintf(sock, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", file, host);
}

char* GetHeader(char* buffer, char* template) {
  char* begin = strstr(buffer, template) + strlen(template) + 2;
  char* end = strstr(begin, "\r\n");

  int size = end - begin;
  char* result = malloc(size + 1);
  memcpy(result, begin, size);
  result[size] = '\0';
  return result;
}

int main(int argc, char* argv[])
{
    struct hostent* host = gethostbyname(argv[1]);
    const char* ip = inet_ntoa(**((struct in_addr**)host->h_addr_list));

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr(ip);
    // printf("%s\n", ip);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        printf("Error connect\n");
        return 1;
    }

    const int N = 1024;

    char buffer[N + 1];
    int size = 0;

    SendGetReq(sock, argv[1], argv[2]);

    char text_begin = 0;
    int content_length = 0;
    while ((size = read(sock, buffer, N)) > 0) {
        buffer[size] = '\0';

        char* start_body = buffer;
        if (text_begin == 0) {
            start_body = strstr(buffer, "\r\n\r\n") + 4;

            /*
            const char* cont_len = "Content-Length: ";
            const char* beg_con_len = strstr(buffer, cont_len) + sizeof(cont_len);
            const char* end_con_len = strstr(beg_con_len, "\r\n");
            *end_con_len = '\0';
            */

            content_length = atoi(GetHeader(buffer, "Content-Length"));

            text_begin = 1;
        }

        content_length -= strlen(start_body);

        printf("%s", start_body);

        if (content_length == 0) {
          break;
        }
    }
}
