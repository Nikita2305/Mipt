#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int sockfd = 0, connfd = 0;
struct sockaddr_in serv_addr;

void GetFileName(char* message, char* filename)
{
    message += 4; // skip GET and space
    int i = 0;
    while (message[i] != ' ') {
        filename[i] = message[i];
        ++i;
    }
    filename[i] = 0;
}

void GetNotFound(char* message)
{
    const char* str = "HTTP/1.1 404 Not Found\r\n\0";
    memcpy(message, str, strlen(str));
}

void GetForbidden(char* message)
{
    const char* str = "HTTP/1.1 403 Forbidden\r\n\0";
    memcpy(message, str, strlen(str));
}

volatile sig_atomic_t has_connection = 0;

void HandleFatal(int sugnum)
{
    if (!has_connection) {
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        exit(0);
    } else {
        has_connection = 2;
    }
}

struct sigaction action_sigint;
struct sigaction action_sigterm;

void SetupSignalHandlers()
{
    memset(&action_sigint, 0, sizeof(action_sigint));
    action_sigint.sa_handler = HandleFatal;
    action_sigint.sa_flags = SA_RESTART;
    sigaction(SIGINT, &action_sigint, NULL);

    memset(&action_sigterm, 0, sizeof(action_sigterm));
    action_sigterm.sa_handler = HandleFatal;
    action_sigterm.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &action_sigterm, NULL);
}

int main(int argc, char* argv[])
{
    SetupSignalHandlers();

    if (argc != 3) {
        return 0;
    }
    int port = strtol(argv[1], NULL, 10);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return 0;
    }

    int option = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        return 0;
    }
    if (listen(sockfd, 1024) < 0) {
        return 0;
    }
    while (has_connection != 2) {
        has_connection = 0;
        connfd = accept(sockfd, (struct sockaddr*)NULL, NULL);
        has_connection = 1;
        char sendBuff[4000];
        char filepath[1000];
        char filename[1000];
        memset(sendBuff, 0, sizeof(sendBuff));
        memset(filepath, 0, sizeof(filepath));
        memset(filename, 0, sizeof(filename));

        if (read(connfd, sendBuff, sizeof(sendBuff)) <= 0) {
            break;
        }
        memcpy(filepath, argv[2], strlen(argv[2]));
        GetFileName(sendBuff, filename);
        char* file = 0;
        if (filename[0] != '/') {
            file = strcat(filepath, "/");
        } else {
            file = filepath;
        }
        file = strcat(file, filename);
        memset(sendBuff, 0, sizeof(sendBuff));
        if (access(file, F_OK) == -1) {
            GetNotFound(sendBuff);
        } else if (access(file, R_OK) == -1) {
            GetForbidden(sendBuff);
        } else {
            int fd = open(file, O_RDONLY);
            char readBuffer[1000];
            int sz = read(fd, readBuffer, sizeof(readBuffer));
            assert(sz >= 0);
            readBuffer[sz] = 0;
            snprintf(
                sendBuff,
                sizeof(sendBuff),
                "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%s",
                sz,
                readBuffer);
        }

        write(connfd, sendBuff, strlen(sendBuff));
        shutdown(connfd, SHUT_RDWR);
        close(connfd);
    }

    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    return 0;
}
