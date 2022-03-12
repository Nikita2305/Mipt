#include <sys/types.h>
#include <sys/syscall.h>

extern long syscall(long syscallno, ...);

ssize_t write(int fd, const void* buf, size_t count) {
    return syscall(SYS_write, fd, buf, count);
} 

ssize_t read(int fd, void* buf, size_t count) {
    return syscall(SYS_read, fd, buf, count);
}

int READ = 5000;

int main() {
    char string[READ];  
    int just_read;   
    while ((just_read = read(0, string, READ)) != 0) {
        if (just_read == -1) {
            return 0;
        }
        write(1, string, just_read);
    }
}

void my_exit(int status) {
    syscall(SYS_exit, status);
}

/*void _start() {
    my_exit(main());
}*/
