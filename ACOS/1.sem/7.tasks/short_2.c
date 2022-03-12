#include <sys/syscall.h>
#include <sys/types.h>

extern long syscall(long syscallno, ...);

void _start()
{
    char string[1];
    while (syscall(SYS_read, 0, string, 1) == 1) {
        syscall(SYS_write, 1, string, 1);
    }
    syscall(SYS_exit, 0);
}
