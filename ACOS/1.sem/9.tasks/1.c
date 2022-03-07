#define __USE_FILE_OFFSET64
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    const int length = PATH_MAX;
    char path[length];
    long long size_of_regulars = 0;
    while (fgets(path, length, stdin)) {
        char* end_of_string = memchr(path, '\n', length);
        if (end_of_string) {
            *end_of_string = '\0';
        }
        if (access(path, F_OK | R_OK) == -1) {
            continue;
        }
        struct stat sb;
        lstat(path, &sb);
        if (S_ISREG(sb.st_mode)) {
            size_of_regulars += sb.st_size;
        }
    }
    printf("%lld", size_of_regulars);
    return 0;
}
