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
#include <sys/epoll.h>
#include <ctype.h>

int SIGTERM_indicator;
int sfd;
int efd;

void SIGTERM_handler(int signum)
{
    shutdown(sfd, SHUT_RDWR);
    close(efd);
    close(sfd);
    exit(0);
}

int main(int argv, char** argc)
{
    // Setting up signal handler
    struct sigaction term_handler;
    memset(&term_handler, 0, sizeof(term_handler));
    term_handler.sa_handler = SIGTERM_handler;
    term_handler.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &term_handler, NULL);
    SIGTERM_indicator = 0;

    // Setting up a socked fd     
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argc[1]));
    addr.sin_addr.s_addr = htonl(0);

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sfd, F_SETFL, fcntl(sfd, F_GETFL) | O_NONBLOCK);
    int res = bind(sfd, (struct sockaddr*)&addr, sizeof(addr));
    if (res < 0) {
        perror("bind error");
        return 1;
    }

    res = listen(sfd, SOMAXCONN);
    if (res < 0) {
        perror("listen error");
        return 1;
    }

    // Setting up polling
    efd = epoll_create1(0);
    if (efd < 0) {
        perror("poll error");
        return 1;
    }
    struct epoll_event temp_event;
    temp_event.data.fd = sfd;
    temp_event.events = EPOLLIN;
    res = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &temp_event);
    if (res < 0) {
        perror("epoll_ctl error");
        return 1;
    }
     
    // Polling process
    int events_bunch = 16;
    struct epoll_event events[events_bunch];
    while (1) {  
        int n = epoll_wait(efd, events, events_bunch, -1);
        for (int event_i = 0; event_i < n; event_i++) {
            struct epoll_event event = events[event_i];
            if (sfd == event.data.fd) {
                int infd = accept(sfd, NULL, NULL);
                if (infd == -1) {
                    if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                        continue;
                    } else {
                        perror("accept");
                        continue;
                    }
                }

                fcntl(sfd, F_SETFL, fcntl(sfd, F_GETFL) | O_NONBLOCK);            
                temp_event.data.fd = infd;
                temp_event.events = EPOLLIN;
                res = epoll_ctl(efd, EPOLL_CTL_ADD, infd, &temp_event);
                if (res < 0) {
                    perror("epoll_ctl in cycle");
                    return 1;
                }
            } else {
                int done = 0;

                const int buffer_size = 1024;
                char buffer[buffer_size];

                int just_read = read(event.data.fd, buffer, buffer_size);
                if (just_read == -1) {
                    if (errno != EAGAIN) {
                        perror("read");
                        done = 1;
                    }
                    continue;
                } else if (just_read == 0) {
                    done = 1;
                    continue;
                } else {
                    for (int i = 0; i < just_read; i++) {
                        buffer[i] = toupper(buffer[i]);
                    }
                    // write(1, buffer, just_read);
                    res = write(event.data.fd, buffer, just_read);
                    if (res == -1) {
                        perror("write error");
                        return 1;
                    }
                }
                if (done) {
                    close(event.data.fd);
                }
            }
        }
    }
}
