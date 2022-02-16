#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int SIGINT_counter;
int SIGTERM_indicator;

void SIGINT_handler(int signum)
{
    SIGINT_counter++;
}

void SIGTERM_handler(int signum)
{
    SIGTERM_indicator = 1;
}

int main()
{
    struct sigaction int_handler;
    memset(&int_handler, 0, sizeof(int_handler));
    int_handler.sa_handler = SIGINT_handler;
    int_handler.sa_flags = SA_RESTART;
    sigaction(SIGINT, &int_handler, NULL);

    struct sigaction term_handler;
    memset(&term_handler, 0, sizeof(term_handler));
    term_handler.sa_handler = SIGTERM_handler;
    term_handler.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &term_handler, NULL);

    SIGINT_counter = 0;
    SIGTERM_indicator = 0;

    pid_t self_pid = getpid();
    printf("%d\n", self_pid);
    fflush(stdout);

    while (SIGTERM_indicator == 0) {
        pause();
    }
    printf("%d", SIGINT_counter);
}
