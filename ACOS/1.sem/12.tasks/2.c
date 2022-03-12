#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int maxValue = 100;
int maxProgram = 1e4;

int main() {
    char value[maxValue];  
    char program[maxProgram]; 
    fgets(value, maxValue, stdin);
    int i = strlen(value) - 1;
    while (i >= 0 && value[i] == '\n') {
        value[i] = '\0';
        i--;
    }
    sprintf(program, "#include <stdio.h>\nint main() {printf(\"%%d\", (int)(%s));}", value);
    int fd = open("temp.c", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    write(fd, program, strlen(program)); 
    close(fd);
    pid_t pid = fork();
    if (pid == 0) {
        execlp("gcc", "gcc", "temp.c", "-o", "a.out", NULL);
        return 0;
    } else {
        int ret = 0;
        waitpid(pid, &ret, 0);
    }
    execlp("./a.out", "a.out", NULL);
}
