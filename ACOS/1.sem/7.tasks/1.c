#include <sys/types.h>
#include <sys/syscall.h>

extern long syscall(long syscallno, ...);

ssize_t write(int fd, const void* buf, size_t count) {
    return syscall(SYS_write, fd, buf, count);
} 

int main() {
    static const char string[] = "Hello world!";
    write(1, string, sizeof(string) - 1);
}

void my_exit(int status) {
    syscall(SYS_exit, status);
}

void _start() {
    my_exit(main());
}
