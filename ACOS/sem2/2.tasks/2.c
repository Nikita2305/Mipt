#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <sys/stat.h>
#include <fcntl.h>

int TERM_indicator;
int N = 0;
char** arr;

void SIGUSR_handler(int signum) {
    if (TERM_indicator == 1 || signum - SIGRTMIN == 0) {
        TERM_indicator = 1;
        return;
    }
    int x = signum - SIGRTMIN;
    if (x > N) {
        return;
    }
    int fd = open(arr[x], O_RDONLY);
    int just_read = 0;
    const int temp_read = 1024;
    char buf[temp_read];
    while ((just_read = read(fd, buf, temp_read)) > 0) {
        write(1, buf, just_read);
    }
    close(fd); 
}

int main(int argc, char* argv[]) {
    N = argc - 1;
    arr = argv; 
    struct sigaction usr_handler;
    memset(&usr_handler, 0, sizeof(usr_handler));
    usr_handler.sa_handler = SIGUSR_handler;
    usr_handler.sa_flags = SA_RESTART;
    sigset_t fullmask;
    sigfillset(&fullmask);
    usr_handler.sa_mask = fullmask;

    sigset_t mask;
    sigfillset(&mask);
    for (int i = SIGRTMIN; i <= SIGRTMAX; i++) {
        sigaction(i, &usr_handler, NULL);
        sigdelset(&mask, i); 
    }
    sigprocmask(SIG_SETMASK, &mask, NULL);

    TERM_indicator = 0;    
    
    pid_t self_pid = getpid();
    printf("%d\n", self_pid);
    fflush(stdout);

    int fd = open("temp.txt", O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if (fd == -1) {
	    printf("Wassup\n");
	    return 1;
    }
    dup2(fd, 1);
    
    while (TERM_indicator == 0) {
        pause();
    }
}
