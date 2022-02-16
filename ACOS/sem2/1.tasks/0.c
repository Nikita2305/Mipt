#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int clean_pipe(int channel[2]) {
    int res = 0, just_read = 0;
    const int temp_read = 1024;
    char buf[temp_read];
    while ((just_read = read(channel[0], buf, temp_read)) != 0) {
        if (just_read == -1) {
            return -1;
        }
        res += just_read;
    }
    return res;
}

int main(int argv, char** args) {
    int fd = open(args[2], O_RDONLY);
    if (fd == -1) {
        return 1;
    }
    int channel[2];
    int res = pipe(channel);
    if (res == -1) {
        return 1;
    }
    pid_t pid = fork();
    if (pid == -1) {
        return 1;
    } else if (pid == 0) {
        res = close(channel[0]);
        if (res == -1) {
            return 1;
        }
        res = dup2(channel[1], 1); // Write to the pipe
        if (res == -1) {
            return 1;
        }
        res = dup2(fd, 0); // Read from the file
        if (res == -1) {
            return 1;
        }
        res = close(channel[1]);
        if (res == -1) {
            return 1;
        }
        res = close(fd);
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
    if (status == 1 || res == -1) {
        return 1;
    }
    res = close(channel[1]);
    if (res == -1) {
        return 1;
    }
    int just_read = clean_pipe(channel);
    if (just_read == -1) {
        return 1;
    }
    printf("%d", just_read);
    res = close(channel[0]);
    if (res == -1) {
        return 1;
    }
}
