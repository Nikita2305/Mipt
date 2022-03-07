#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int substitute(int fd, int new_fd)
{
    int res = dup2(new_fd, fd);
    if (res == -1) {
        return -1;
    }
    res = close(new_fd);
    if (res == -1) {
        return -1;
    }
    return 0;
}

int main(int argv, char** args)
{
    int channel[2];
    int res = pipe(channel);
    if (res == -1) {
        return 1;
    }
    // Run CMD1
    pid_t pid = fork();
    if (pid == -1) {
        return 1;
    } else if (pid == 0) {
        res = close(channel[0]);
        if (res == -1) {
            return 1;
        }
        res = substitute(1, channel[1]);
        if (res == -1) {
            return 1;
        }
        res = execlp(args[1], args[1], NULL);
        if (res == -1) {
            return 1;
        }
        _exit(0);
    }
    int status = 0;
    res = waitpid(pid, &status, 0);
    if (res == -1 || WEXITSTATUS(status) == 1) {
        return 1;
    }
    res = close(channel[1]);
    if (res == -1) {
        return 1;
    }
    // Run CMD2
    pid = fork();
    if (pid == -1) {
        return 1;
    } else if (pid == 0) {
        res = substitute(0, channel[0]);
        if (res == -1) {
            return 1;
        }
        res = execlp(args[2], args[2], NULL);
        if (res == -1) {
            return 1;
        }
        _exit(0);
    }
    status = 0;
    res = waitpid(pid, &status, 0);
    if (res == -1 || status == 1) {
        return 1;
    }
    close(channel[0]);
    close(channel[1]);
}
