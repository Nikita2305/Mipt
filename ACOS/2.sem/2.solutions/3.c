#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int TERM = 1;

void handler(int sig, siginfo_t* info, void* ucontext)
{
    if (info->si_value.sival_int == 0) {
        TERM = 0;
        return;
    }
    info->si_value.sival_int -= 1;
    sigqueue(info->si_pid, sig, info->si_value);
}

int main()
{
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGRTMIN);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    struct sigaction usr_handler;
    memset(&usr_handler, 0, sizeof(usr_handler));
    usr_handler.sa_sigaction = handler;
    usr_handler.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGRTMIN, &usr_handler, NULL);

    while (TERM) {
        pause();
    }
}
