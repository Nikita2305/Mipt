#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    // printf("hello\n");
    int fd1 = open(argv[1], O_RDONLY),
        fd2 = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0666),
        fd3 = open(argv[3], O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if (fd1 == -1) {
        return 1;
    } else if (fd2 == -1 || fd3 == -1) {
        return 2;
    }
    // printf("%d %d %d\n", fd1, fd2, fd3);
    const int read_size = 4000;
    int just_read = 0;

    char input[read_size];
    char numbers[read_size];
    char other[read_size];
    while (just_read = read(fd1, input, read_size)) {
        // printf("%d\n", just_read);
        if (just_read == -1) {
            return 3;
        }
        int numbers_size = 0;
        int other_size = 0;
        for (int i = 0; i < just_read; i++) {
            if (input[i] >= '0' && input[i] <= '9') {
                numbers[numbers_size] = input[i];
                numbers_size++;
            } else {
                other[other_size] = input[i];
                other_size++;
            }
        }
        int numbers_written = write(fd2, numbers, numbers_size);
        int other_written = write(fd3, other, other_size);
        if (numbers_written != numbers_size || other_written != other_size) {
            return 3;
        }
    }
    close(fd1), close(fd2), close(fd3);
    // printf("bye");
    return 0;
}
