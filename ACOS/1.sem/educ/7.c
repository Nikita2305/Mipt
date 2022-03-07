#include <sys/types.h>
#include <sys/syscall.h>

extern long syscall(long syscallno, ...);

ssize_t write(int fd, const void* buf, size_t count) {
    long res = syscall(1, fd, buf, count); // 1 = write number
    return res;
}

int main() {
    static const char Hello[] = "Hello\n";
    write(1, Hello, sizeof(Hello) - 1); //To not write \0
    // there was another command to write chars,
    // but that[write] could write:)) even binary data, as there is void*
}

void _start() {
    int ret = main();
    _exit(ret);
}
// ldd ./hello - shows dependencies
// man 2 exit - сисколл //_exit
// man exit - из стандартной либы
    // вызывает syscall exit

void exit(int status) {
    syscall(SYS_exit, status); // SYS_exit = 60
}
// to know syscall number:
    // cat /usr/include/asm/unistd.h | grep exit
    // 60
    // OR "strace" program ?
// btw with stdlib _start calls exit_group that works exits all threads


// man 2 brc // sbrk просто системный вызов brk
// brk перемещает границу между стеком и кучей и возвращает новый адрес
    // ~ aka malloc
// brk(0) - возвращает нынешний адрес 
// brk(+eps) - выделение памяти
// brk(-eps) - освобождение памяти
// brk(x) ~~ some_ptr += x

// stack растёт сверху вниз
// куча растёт снизу вверх

// -------------------syscall.s---------------------
/*
    .intel_syntax noprefix
    .text
    .global my_syscall
    
my_syscall:
    # rdi, rsi, rdx, r10, r8, r9
    # rax <- number = rdi
    # rdi <- arg1   = rsi
    # rsi <- arg2   = rdx 
    # rdx <- arg3   = rcx
    mov rax, rdi
    mov rdi, rsi
    mov rsi, rdx
    mov rdx, rcx
    syscall
    ret
*/

// gcc --nostblib -o hello hello.c syscall.S

