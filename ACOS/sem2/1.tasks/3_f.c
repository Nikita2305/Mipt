#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argv, char** args)
{
    int channel_read[2], channel_write[2];
    channel_read[0] = 0; // open to read from console
    channel_read[1] = -1; 
    for (int i = 1; i < argv; i++) {
        if (i == argv - 1) { 
            channel_write[0] = -1;
            channel_write[1] = 1;
        } else {
            pipe(channel_write);
        }
        // State: channel_read[1] is closed
        // channel_write[1] and channel_read[0] are open
        // probably channel_write[0] is closed. 
        pid_t pid = fork();
        if (pid == 0) {
            dup2(channel_read[0], 0);
            dup2(channel_write[1], 1);
            execlp(args[i], args[i], NULL);
            return 0;
        }
        int status = 0;
        waitpid(pid, &status, 0);
        if (i > 1) {
            close(channel_read[0]);
        }
        // State: channel_read is closed anyway.
        if (i < argv - 1) { // I should not do anything, if its stdout
            channel_read[0] = channel_write[0];
            channel_read[1] = channel_write[1];
            close(channel_read[1]);
        }
    }
    return 0;
}
