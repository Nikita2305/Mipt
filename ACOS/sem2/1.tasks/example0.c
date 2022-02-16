#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

int main(int argv, char** arr) {
    int channel[2];
    pipe(channel);
    pid_t pid = fork();
    if (pid == 0) {
        close(channel[0]);
        dup2(channel[1], 1);
        close(channel[1]);
        execlp("ls", "ls", NULL);
        _exit(0);
    }
    close(channel[1]);
    char buf[1024];
    int just_read = read(channel[0], buf, 1024);
    write(1, buf, just_read);
    close(channel[0]);
}
