#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    pid_t pid;
    int processes = 0;
    do {
        pid = fork();
        processes++;
        if (pid == 0) {
            sched_yield();
            return 0;
        }
    } while (-1 != pid);
    printf("%d", processes);
}
