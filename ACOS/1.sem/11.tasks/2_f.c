#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return 1;
    }
    int N = atoi(argv[1]);
    if (N <= 0) {
        return 1;
    }
    for (int i = 1; i <= N; i++) {
        printf("%d", i);
        if (i == N) {
            printf("%s", "\n");
            return 0;
        } else {
            printf("%s", " ");
        }
        fflush(stdout);
        pid_t pid = fork();
        if (pid == -1) {
            return 1;
        } else if (pid != 0) {
            int res = 0;
            waitpid(pid, &res, 0);
            return WEXITSTATUS(res);
        }
    }
}
