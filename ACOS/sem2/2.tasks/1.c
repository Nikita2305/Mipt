#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>

#include <sys/stat.h>
#include <fcntl.h>

int core_value;
int SIGTERM_indicator;

int fill(char* arr, int arr_size, int value) {
    int minus = (value < 0);
    if (minus) {
        value = -value;
    }
    int index = arr_size - 1;
    while (value > 0) {
        arr[index] = '0' + (value % 10);
        value /= 10;
        index--;
    }
    if (index == arr_size - 1) {
        arr[index] = '0';
        index--;
    }
    if (minus) {
        arr[index] = '-';
        index--;
    }
    return index + 1;
}

void SIGUSR_handler(int signum) {
    if (SIGTERM_indicator == 1) {
        return;
    }
    if (signum == SIGUSR1) {
        core_value += 1;
    } else {
        core_value *= -1;
    }
    const int buffer_size = 20;
    char buffer[buffer_size];
    buffer[buffer_size - 1] = '\n';
    int index = fill(buffer, buffer_size - 1, core_value);
    write(1, buffer + index, buffer_size - index); 
}

void SIGTERM_handler(int signum) {
    SIGTERM_indicator = 1;
}

int main() {
    struct sigaction term_handler;
    memset(&term_handler, 0, sizeof(term_handler));
    term_handler.sa_handler = SIGTERM_handler;
    term_handler.sa_flags = SA_RESTART;
    sigaction(SIGINT, &term_handler, NULL);
    sigaction(SIGTERM, &term_handler, NULL);
    
    struct sigaction usr_handler;
    memset(&usr_handler, 0, sizeof(usr_handler));
    usr_handler.sa_handler = SIGUSR_handler;
    usr_handler.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &usr_handler, NULL);
    sigaction(SIGUSR2, &usr_handler, NULL);
     
    SIGTERM_indicator = 0;   
    pid_t self_pid = getpid();
    printf("%d\n", self_pid);
    fflush(stdout);
   
    /*int fd = open("temp.txt", O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if (fd == -1) {
        printf("Wassup\n");
        return 1;
    }
    dup2(fd, 1);
    */ 

    core_value = 0;
    scanf("%d", &core_value);
    
    while (SIGTERM_indicator == 0) {
        pause();
    }
}
