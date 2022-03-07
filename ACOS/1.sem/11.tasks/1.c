#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

int main()
{
    pid_t pid;
    int processes = -1;
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
