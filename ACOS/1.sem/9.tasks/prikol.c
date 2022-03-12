#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
 
const int N = PATH_MAX + 1;
 
void normalize_path(char* path)
{
    char abs_path = 0;
    if (*path == '/') {
        abs_path = 1;
        ++path;
    }
    char* begin = path;
    char* current = path;
 
    size_t count_pies = 0;
    int state = 0; // 0 - empty, 1 - ., 2 - .., 3 - other
    while (1) {
        *current = *path;
        ++current;
 
        if (*path == '/') {
            if (state == 0) {
                --current;
            } else if (state == 1) {
                current -= 2;
            } else if (state == 2) {
                if (count_pies == 0) {
                    if (abs_path) {
                        current -= 3; //           /../ -> /
                    }
                } else {
                    current -= 5;
                    while (*current != '/' && current != begin) {
                        --current;
                    }
                    if (current != begin) {
                        ++current;
                    }
                    --count_pies;
                }
            } else if (state == 3) {
                ++count_pies;
            }
            state = 0;
        } else if (*path == '\0') {
            if (state == 0) {
                // norhing
            } else if (state == 1) {
                current -= 1;
                *current = '\0';
            } else if (state == 2) {
                if (count_pies == 0) {
                    if (abs_path) {
                        current -= 3; //           "/../" -> "/"
                    }
                } else {
                    current -= 5;
                    while (*current != '/' && current != begin) {
                        --current;
                    }
                    if (current != begin) {
                        ++current;
                    }
                    --count_pies;
                    *current = '\0';
                }
            } else if (state == 3) {
                ++count_pies;
            }
            state = 0;
            return;
        } else if (*path == '.') {
            if (state == 0) {
                state = 1;
            } else if (state == 1) {
                state = 2;
            } else {
                state = 3;
            }
        } else {
            state = 3;
        }
        ++path;
    }
}
 
int main(int argc, char* argv[])
{
    char* name_file = malloc(2 * N);
    char* link_file = malloc(N);
    char* pwd = malloc(N + 1);
    getcwd(pwd, N);
    size_t size_pwd = strlen(pwd);
    pwd[size_pwd] = '/';
    ++size_pwd;
 
    while (fgets(name_file, N, stdin)) {
        int n = strlen(name_file);
        if (name_file[n - 1] == '\n') {
            name_file[n - 1] = '\0';
            n--;
        }
        struct stat stat_file;
        if (lstat(name_file, &stat_file)) {
            continue;
        }
        if (S_ISREG(stat_file.st_mode) != 0) {
            for (size_t i = 0; i < size_pwd; ++i) {
                link_file[i] = pwd[i];
            }
            strcpy(link_file, "link_to_");
            int i = n - 1;
            while (name_file[i] != '/' && i > 0) {
                --i;
            }
            if (name_file[i] == '/') {
                ++i;
            }
            strcpy(link_file + 8, name_file + i);
            symlink(name_file, link_file);
        } else if (S_ISLNK(stat_file.st_mode) != 0) {
            size_t size = readlink(name_file, link_file, N);
            link_file[size] = '\0';
            if (link_file[0] == '/') {
                printf("%s\n", link_file);
            } else {
                int i = n - 1;
                while (name_file[i] != '/' && i > 0) {
                    --i;
                }
                if (name_file[i] == '/') {
                    ++i;
                }
                strcpy(name_file + i, link_file);
                realpath(name_file, link_file);
                printf("%s\n", link_file);
            }
        }
    }
 
    free(name_file);
    free(link_file);
    free(pwd);
 
    return 0;
}
