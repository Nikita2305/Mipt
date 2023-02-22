#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

volatile sig_atomic_t core_value;
volatile sig_atomic_t SIGTERM_indicator;
volatile sig_atomic_t SIGUSR_indicator;

void SIGUSR_handler(int signum)
{
    if (SIGTERM_indicator == 1) {
        return;
    }
    SIGUSR_indicator = signum; 
}

void SIGTERM_handler(int signum)
{
    SIGTERM_indicator = 1;
}

int main()
{
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

    core_value = 0;
    scanf("%d", &core_value);

    while (SIGTERM_indicator == 0) {
        if (SIGUSR_indicator) { 
            if (SIGUSR_indicator == SIGUSR1) {
                core_value += 1;
            } else {
                core_value *= -1;
            }
            printf("%d\n", core_value);
            fflush(stdout);
            SIGUSR_indicator = 0;
        }
        pause();
    }
}
