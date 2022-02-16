#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int obtain_row(char* buf) {
    char* pos1 = strstr(buf, ":");
    char* pos2 = strstr(pos1 + 1, ":");
    pos1++;
    int res = 0;
    while (pos1 != pos2) {
        res = res * 10 + ((*pos1) - '0');
        pos1++;
    }
    return res;
}

int find_patterns(int fd, int* warns, int* errors) {
    *warns = 0;
    char* warn_str = "warning:";
    int C_WARN = 1;
    *errors = 0;
    char* error_str = "error:";
    int C_ERROR = 2;
    FILE* fp = fdopen(fd, "r");
    size_t n = 0; 
    const int row_count = 10000;
    int rows[row_count];
    for (int i = 0; i < row_count; i++) {
        rows[i] = 0;
    }
    while (1) {
        char* buf = NULL;
        int res = getline(&buf, &n, fp);
        if (res == -1) {
            free(buf);
            break;
        }
        char* pos_w = strstr(buf, warn_str);
        if (pos_w) {
            rows[obtain_row(buf)] |= C_WARN;
        }
        char* pos_e = strstr(buf, error_str);
        if (pos_e) {
            rows[obtain_row(buf)] |= C_ERROR;
        }
        free(buf);
    }
    for (int i = 0; i < row_count; i++) {
        *warns += ((rows[i] & C_WARN) != 0);
        *errors += ((rows[i] & C_ERROR) != 0);
    }
    return 0;
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
        res = substitute(2, channel[1]);
        if (res == -1) {
            return 1;
        }
        res = execlp("gcc", "gcc", args[1], NULL);
        if (res == -1) {
            return 1;
        }
        _exit(0);
    }
    int status = 0;
    res = waitpid(pid, &status, 0);
    if (res == -1) {
        return 1;
    }
    res = close(channel[1]);
    if (res == -1) {
        return 1;
    }
    int warns, errors;
    res = find_patterns(channel[0], &warns, &errors);
    if (res == -1) {
        return 1;
    }
    printf("%d %d", errors, warns);
    res = close(channel[0]);
    if (res == -1) {
        return 1;
    }
}
