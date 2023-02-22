#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

struct config {
    int send_first;
    int core_value;
    int fd;
    int (*modifier)(int);
};

int UPPER_BOUND = 100;
int BANNED_VALUE = 0;
int END_OF_INPUT = -100;
int buffer_size = 8;

void send_value(int fd, int value) {
    char buffer[8];
    int just_written = sprintf(buffer, "%d", value);
    write(fd, buffer, just_written);
}

int convert_and_send(int value, int (*modifier)(int), int fd) {
    value = modifier(value);
    printf("%d\n", value);
    if (value == BANNED_VALUE || value > UPPER_BOUND) { 
        send_value(fd, END_OF_INPUT);
        return -1;
    }
    send_value(fd, value);
    return 0;
}

void* transform(void* info) {
    struct config* conf = (struct config*)info;
    int value = conf->core_value;
    if (conf->send_first) {
        if (convert_and_send(value, conf->modifier, conf->fd) != 0) {
            return NULL;
        } 
    }
    while (1) {
        char buffer[buffer_size + 1];
        int just_read = read(conf->fd, buffer, buffer_size);
        if (just_read <= 1) {
            perror("read");
            break;
        }
        buffer[just_read] = '\0';
        value = atoi(buffer);
        if (value == END_OF_INPUT) {
            break;
        }
        if (convert_and_send(value, conf->modifier, conf->fd) != 0) {
            break;
        } 
    }
    return NULL;
}

int increaser(int value) {
    return value + 5;
}

int decreaser(int value) {
    return value - 3;
}

int main(int argc, char** argv) {
    int N = atoi(argv[1]);
    pthread_t threads[2];
    int pair[2];
    if (0 != socketpair(AF_UNIX, SOCK_STREAM, 0, pair)) {
        perror("socketpair");
    }
    struct config thread_config1 = {.send_first = 1,
                                    .core_value = N,
                                    .fd = pair[0],
                                    .modifier = &decreaser};
    if (0 != pthread_create(&threads[0], NULL, &transform, &thread_config1)) {
        perror("pthread_create");
    }
    struct config thread_config2 = {.send_first = 0,
                                    .core_value = N,
                                    .fd = pair[1],
                                    .modifier = &increaser};
    if (0 != pthread_create(&threads[1], NULL, &transform, &thread_config2)) {
        perror("pthread_create");
    }
    if (0 != pthread_join(threads[0], NULL)) {
        perror("pthread_join");
    }
    if (0 != pthread_join(threads[1], NULL)) {
        perror("pthread_join");
    }
    close(pair[0]);
    close(pair[1]);
}
