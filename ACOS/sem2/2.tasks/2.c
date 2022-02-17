#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/signalfd.h>

int MAX_LENGTH = 4000;

int main(int argc, char* argv[]) { 
    sigset_t full_mask;
    sigfillset(&full_mask);
    sigprocmask(SIG_SETMASK, &full_mask, NULL);    
    sigset_t mask;
    sigemptyset(&mask);
    for (int i = SIGRTMIN; i <= SIGRTMAX; i++) {
        sigaddset(&mask, i); 
    }
    int sfd = signalfd(-1, &mask, 0); 

    FILE* files[argc];
    for (int i = 1; i < argc; i++) {
        files[i] = fopen(argv[i], "r");
    }

    while(1) {
        struct signalfd_siginfo signal;
        read(sfd, &signal, sizeof(signal));
        int x = signal.ssi_signo - SIGRTMIN;
        if (x == 0) {
            break;
        }
        if (x > argc) {
            continue;
        }
        char string[MAX_LENGTH];
        if (fgets(string, MAX_LENGTH, files[x])) {
            fputs(string, stdout);
        } else { 
            fputs("\n", stdout);
        }
        fflush(stdout);
    }
    close(sfd);
    for (int i = 1; i < argc; i++) {
        fclose(files[i]);
    }
}
