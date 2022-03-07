#include <sys/syscall.h>
#include <sys/types.h>

extern long syscall(long syscallno, ...);

ssize_t write(int fd, const void* buf, size_t count)
{
    return syscall(SYS_write, fd, buf, count);
}

ssize_t read(int fd, void* buf, size_t count)
{
    return syscall(SYS_read, fd, buf, count);
}

int main()
{
    char string[1];
    while (read(0, string, 1) == 1) {
        write(1, string, 1);
    }
}

void my_exit(int status)
{
    syscall(SYS_exit, status);
}

void _start()
{
    my_exit(main());
}
