#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>

extern long syscall(long syscallno, ...);

ssize_t write(int fd, const void* buf, size_t count) {
    return syscall(SYS_write, fd, buf, count);
} 

ssize_t read(int fd, void* buf, size_t count) {
    return syscall(SYS_read, fd, buf, count);
}

void* gea() {
    return (void*)syscall(SYS_brk, 0);
}

int brk(void* addr) {
  return syscall(SYS_brk, addr);
}

void _start() {
    char string[] = "kek"; 
    int size = 0;
    char* input = gea();
    brk(gea() + 1); 
    if (read(0, input, 1) == 1) {
        write(1, string, 3);
    }
    syscall(SYS_exit, 0);
}
