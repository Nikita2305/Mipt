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

volatile sig_atomic_t SIGTERM_indicator;
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

int prepare_signals(struct sigaction* term_handler) {
    memset(term_handler, 0, sizeof(struct sigaction));
    term_handler->sa_handler = SIGTERM_handler;
    term_handler->sa_flags = SA_RESTART;
    if (-1 == sigaction(SIGTERM, term_handler, NULL)) {
        perror("sigaction");
        return -1;
    }
    if (-1 == sigaction(SIGINT, term_handler, NULL)) {
        perror("sigaction");
        return -1;
    }
    SIGTERM_indicator = 0;
    is_accepting = 0;
    return 0;
}

int prepare_socket(struct sockaddr_in* addr, uint16_t port) {
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = htonl(0);

    sfd = socket(AF_INET, SOCK_STREAM, 0);

    int res = bind(sfd, (struct sockaddr*)addr, sizeof(struct sockaddr_in));
    if (res < 0) {
        perror("bind");
        return -1;
    }

    res = listen(sfd, 128);
    if (res < 0) {
        perror("listen");
        return -1;
    }
    return 0;
}

int handle_connection(int fd, char* path) {
    FILE* f = fdopen(fd, "r+");
    char file_name[ARRAY_SIZE];
    for (int i = 0;; i++) {
        char buffer[ARRAY_SIZE];
        int cur_size = ARRAY_SIZE;
        char* cur = buffer;
        int has_data = 1;
        while (1) {
            char* res = fgets(cur, cur_size, f);
            if (res == NULL) {
                has_data = 0;
                break;
            }
            int len = strlen(cur);
            if (cur[len - 2] == '\r') {
                break;
            }
            cur = cur + len;
            cur_size -= len;
        }
        if (has_data == 0) {
            break;
        }
        if (strlen(buffer) == 2) {
            break;
        }
        if (i == 0) {
            *strstr(buffer, " HTTP/1.1") = '\0';
            strcpy(file_name, buffer + 4);
        }
    }
    char file_path[ARRAY_SIZE];
    strcpy(file_path, path);
    int len = strlen(path);
    file_path[len] = '/';
    strcpy(file_path + len + 1, file_name);
    char status[128];
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
    }
    int wfd = fd;
    write(wfd, status, strlen(status));
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
    }
    fclose(f);
    return 0;
}

int main(int argv, char** argc)
{
    struct sigaction term_handler;
    if (-1 == prepare_signals(&term_handler)) {
        return 1;
    }

    uint16_t port = atoi(argc[1]);
    char* path = argc[2];
 
    struct sockaddr_in addr;
    if (-1 == prepare_socket(&addr, port)) {
        return 1;
    }

    while (SIGTERM_indicator == 0) {
        is_accepting = 1;
        int fd = accept(sfd, NULL, NULL);
        is_accepting = 0;
        if (-1 == handle_connection(fd, path)) {
            return 1;
        } 
    }

    shutdown(sfd, SHUT_RDWR);
    close(sfd);
}
