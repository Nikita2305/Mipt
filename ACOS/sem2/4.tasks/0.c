#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

extern size_t read_data_and_count(size_t N, int in[N]) { 
    int efd = epoll_create1(0);
    struct epoll_event event[N];
    for (int i = 0; i < N; i++) {
        fcntl(in[i], F_SETFL, fcntl(in[i], F_GETFL) | O_NONBLOCK);
        event[i].data.fd = in[i];
        event[i].events = EPOLLIN;
        int ret = epoll_ctl(efd, EPOLL_CTL_ADD, in[i], &event[i]);
        if (ret != 0) {
            printf("Error. %s\n", strerror(errno));
            return 0;
        }
    }
 
    int files_left = N;
    int events_bunch = 10;
    int ans = 0;
    struct epoll_event events[events_bunch];
    while (files_left > 0) {  
        int n = epoll_wait(efd, events, events_bunch, -1);
        const int buffer_size = 1024;
        char buffer[buffer_size];
        for (int i = 0; i < n; i++) {
            if (events[i].events & EPOLLIN == 0) {
                continue;
            }
            int just_read = read(events[i].data.fd, buffer, buffer_size);
            if (just_read > 0) {
                ans += just_read;
            } else {
                close(events[i].data.fd);
                --files_left;
            }
        }
    }
    close(efd); 
    return ans;
}

int main() {
    int size = 120;
    int array[size];
    for (int i = 0; i < size; i++) {
        int c[2];
        pipe(c);
        if (c[0] < 0) {
            printf("Error. %s\n", strerror(errno));
            return 1;
        }  
        array[i] = c[0];
        dprintf(c[1], "hello");
        close(c[1]);
    }
    printf("%ld", read_data_and_count(size, array));
}
