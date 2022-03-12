#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>

#define N 4096

long syscall(long number, ...);

ssize_t write(int fd, const void* buf, size_t count) {
    return syscall(SYS_write, fd, buf, count);
}

ssize_t read(int fd, void* buf, size_t count) {
    return syscall(SYS_read, fd, buf, count);
}

void __exit(int status) {
  syscall(SYS_exit, status);
}

void* sbrk(intptr_t increment) {
  return (void*)syscall(SYS_brk, increment);
}

int brk(void* addr) {
  return syscall(SYS_brk, addr);
}

int main() {
  size_t current_size = 0;
  size_t count_blocks = 0;
  int count = N;

  char* array = sbrk(0);

  while (count == N) {
    count_blocks++;
    brk(array + current_size + N);

    count = read(0, array + current_size, N);

    current_size += count;
  }

  if (current_size == 0) {
    return 0;
  }

  if (array[current_size - 1] != '\n') {
    array[current_size] = '\n';
    current_size++;
  }

  for (int i = current_size - 1, j = 0; i >= 0; --i, ++j) {
    if (array[i] == '\n') {
      write(1, array + i + 1, j);
      j = 0;
    } else if (i == 0) {
      write(1, array + i, j + 1);
    }
  }

  return 0;
}

void _start() {
  __exit(main());
}

