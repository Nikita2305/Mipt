#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    const int MAX_SIZE = 5000;
    int id = 0;
    char* word = malloc(MAX_SIZE);
    while (1) {
        int res = scanf("%s", word);
        if (res == EOF) {
            if (id == 0) {
                printf("0");
            }
            return 0;
        }
        pid_t pid = fork();
        if (pid == -1) {
            return 1;
        } else if (pid != 0) {
            int res = 0;
            waitpid(pid, &res, 0);
            if (id == 0) {
                printf("%d", WEXITSTATUS(res) + 1);
                return 0;
            }
            return WEXITSTATUS(res) + 1;
        }
        id += 1;
    }
    return 0;
}
