#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1) {
        return 1;
    }
    struct stat statbuf;
    fstat(fd1, &statbuf);
    int size = statbuf.st_size;
    char* data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd1, 0);
    char* search = data;
    char* found;
    int len = size;
    while (len > 0 &&
           (found = memmem(search, len, argv[2], strlen(argv[2]))) != NULL) {
        printf("%lu ", found - data);
        len -= (found - search);
        search = found + 1;
    }
    munmap(data, size);
    close(fd1);
}
