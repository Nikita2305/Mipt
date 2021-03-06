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
#include <sys/wait.h>
#include <unistd.h>

int SIGTERM_indicator;
int sfd;
int is_accepting;

void SIGTERM_handler(int signum)
{
    if (is_accepting) {
        shutdown(sfd, SHUT_RDWR);
        close(sfd);
        exit(0);
    }
    SIGTERM_indicator = 1;
}

const int ARRAY_SIZE = 4096;

int main(int argv, char** argc)
{
    struct sigaction term_handler;
    memset(&term_handler, 0, sizeof(term_handler));
    term_handler.sa_handler = SIGTERM_handler;
    term_handler.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &term_handler, NULL);
    sigaction(SIGINT, &term_handler, NULL);
    SIGTERM_indicator = 0;
    is_accepting = 0;

    char* path = argc[2];

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atol(argc[1]));
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sfd = socket(AF_INET, SOCK_STREAM, 0);

    int res = bind(sfd, (struct sockaddr*)&addr, sizeof(addr));
    if (res < 0) {
        perror("Error1 connecting");
        return 1;
    }

    res = listen(sfd, SOMAXCONN);
    if (res < 0) {
        perror("Error2 connecting");
        return 1;
    }

    while (SIGTERM_indicator == 0) {
        is_accepting = 1;
        int fd = accept(sfd, NULL, NULL);
        is_accepting = 0;

        // Part 1: reading query

        char buffer[ARRAY_SIZE];
        int length = read(fd, buffer, ARRAY_SIZE);
        buffer[length] = '\0';
        char* end = strstr(buffer, " HTTP/1.1");
        if (end == NULL) {
            perror("Bad query");
            return 1;
        }
        *end = '\0';
        char file_name[ARRAY_SIZE];
        strcpy(file_name, buffer + 4);

        char file_path[ARRAY_SIZE];
        strcpy(file_path, path);
        length = strlen(path);
        file_path[length] = '/';
        strcpy(file_path + strlen(path) + 1, file_name);
        length += (1 + strlen(file_name));
        file_path[length] = '\0';

        // Part 2: writing response

        char status[128];
        int can_execute = 0;
        int can_read = 0;
        int can_access = 0;
        if (access(file_path, F_OK) == -1) {
            strcpy(status, "HTTP/1.1 404 Not Found\r\n\0");
        } else if (access(file_path, R_OK) == -1) {
            strcpy(status, "HTTP/1.1 403 Forbidden\r\n\0");
            can_access = 1;
        } else {
            strcpy(status, "HTTP/1.1 200 OK\r\n\0");
            can_read = 1;
            can_access = 1;
            can_execute = (access(file_path, X_OK) == 0);
        }

        int wfd = fd;
        dprintf(wfd, "%s", status);
        if (can_execute) {
            // write(wfd, "\r\n", 2);
            pid_t pid = fork();
            if (pid == 0) {
                dup2(wfd, 1);
                execlp(file_path, file_path, NULL);
            }
            waitpid(pid, NULL, 0);
        } else {
            if (can_access) {
                struct stat st;
                stat(file_path, &st);
                int size = st.st_size;
                dprintf(wfd, "Content-Length: %d\r\n\r\n", size);
            }
            if (can_read) {
                int cur_file = open(file_path, O_RDONLY);
                int bucket_size = ARRAY_SIZE;
                off_t offset = 0;
                while (sendfile(wfd, cur_file, &offset, bucket_size) ==
                       bucket_size) {}
                close(cur_file);
            }
        }

        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
    shutdown(sfd, SHUT_RDWR);
    close(sfd);
}
