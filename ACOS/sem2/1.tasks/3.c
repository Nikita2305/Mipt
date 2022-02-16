#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int print_pipe(int channel[2]) {
    int res = 0, just_read = 0;
    const int temp_read = 1024;
    char buf[temp_read];
    while ((just_read = read(channel[0], buf, temp_read)) != 0) {
        if (just_read == -1) {
            return -1;
        }
        write(1, buf, just_read);
        res += just_read;
    }
    return res;
}

int substitute(int fd, int new_fd) {
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

int main(int argv, char** args) {
    int channel_read[2], channel_write[2]; 
    int res = pipe(channel_read);
    if (res == -1) {
        return 1;
    }
    res = pipe(channel_write);
    if (res == -1) {
        return 1;
    }  
    for (int i = 1; i < argv; i++) {
        pid_t pid = fork();
        res = close(channel_read[1]);
        if (res == -1) {
            return 1;
        }
        if (pid == -1) {
            return 1;
        } else if (pid == 0) {
            res = close(channel_write[0]);
            if (res == -1) {
                return 1;
            }
            res = substitute(0, channel_read[0]);
            if (res == -1) {
                return 1;
            }
            res = substitute(1, channel_write[1]);
            if (res == -1) {
                return 1;
            }
            res = execlp(args[i], args[i], NULL);
            if (res == -1) {
                return 1;
            }
            _exit(0);
        }
        res = waitpid(pid, 0, 0);
        if (res == -1) {
            return 1;
        }
        res = close(channel_read[0]);
        if (res == -1) {
            return 1;
        }
        channel_read[0] = channel_write[0];
        channel_read[1] = channel_write[1];
        res = pipe(channel_write);
        if (res == -1) {
            return 1;
        } 
    } 
    close(channel_write[0]);
    close(channel_read[1]);
    close(channel_write[1]);
    print_pipe(channel_read);
    close(channel_read[0]);
}
