#define _GNU_SOURCE
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

struct Item {
    int value;
    uint32_t next_pointer;
};

int main(int argc, char* argv[])
{
    int fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1) {
        return 1;
    }
    struct stat statbuf;
    fstat(fd1, &statbuf);

    int size = statbuf.st_size;
    if (size == 0)
        return 0;
    char* data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd1, 0);
    char* read_from = data;
    struct Item* just_read;
    while (1) {
        just_read = (struct Item*)(read_from);
        printf("%d ", just_read->value);
        if (just_read->next_pointer == 0) {
            break;
        }
        read_from = data + just_read->next_pointer;
    }
    munmap(data, size);
    close(fd1);
    return 0;
}
