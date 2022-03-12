#include <dirent.h>
#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

// delete "/abacaba" from the back
char* pop_unit(char* ptr, char* start)
{
    while (ptr != start && *ptr != '/') {
        ptr -= 1;
    }
    return ptr;
}

void normalize_path(char* path)
{
    char* write_ptr = path;
    char* read_ptr = path;
    int state = 0;
    while (*read_ptr != '\0') {
        // printf("%d %c\n", state, *read_ptr);
        if (state == 0) { // prev was nothing or /
            int status = 0;
            if (*read_ptr == '/') {
                if (write_ptr == path) { // = start/ or = //
                    status = 1;
                }
                state = 0;
            } else if (*read_ptr == '.') { // = /.
                state = 1;
                status = 1;
            } else { // = /a
                state = 3;
                status = 1;
            }
            if (status) {
                *write_ptr = *read_ptr;
                write_ptr += 1;
            }
        } else if (state == 1) { //prev was .
            int status = 0;
            if (*read_ptr == '/') { // = ./
                write_ptr = pop_unit(write_ptr - 1, path);
                status = 1;
                state = 0;
            } else if (*read_ptr == '.') { // = ..
                state = 2;
                status = 1;
            } else { // = .a
                state = 3;
                status = 1;
            }
            if (status) {
                *write_ptr = *read_ptr;
                write_ptr += 1;
            }
        } else if (state == 2) { //prev was ..
            int status = 0;
            if (*read_ptr == '/') { // = ../
                write_ptr = pop_unit(write_ptr - 1, path);
                write_ptr = pop_unit(write_ptr - 1, path);
                status = 1;
                state = 0;
            } else if (*read_ptr == '.') { // = ...
                state = 3;
                status = 1;
            } else { // = ..a
                state = 3;
                status = 1;
            }
            if (status) {
                *write_ptr = *read_ptr;
                write_ptr += 1;
            }
        } else if (state == 3) { //prev was letter
            int status = 0;
            if (*read_ptr == '/') { // = a/
                status = 1;
                state = 0;
            } else if (*read_ptr == '.') { // = a.
                state = 3;
                status = 1;
            } else { // = ab
                state = 3;
                status = 1;
            }
            if (status) {
                *write_ptr = *read_ptr;
                write_ptr += 1;
            }
        }
        read_ptr += 1;
    }
    *write_ptr = *read_ptr;
}

int get_path(char* path, const int max_length, FILE* stream)
{
    if (fgets(path, max_length, stream) == NULL) {
        return 1;
    }
    char* end_of_string = memchr(path, '\n', max_length);
    if (end_of_string) {
        *end_of_string = '\0';
    }
    return 0;
}

void print(char* c, int length)
{
    for (int i = 0; i < length; i++) {
        printf("%c", (c[i] == '\0' ? '0' : c[i]));
    }
    printf("\n");
}

int main()
{
    const int length = PATH_MAX + 10;
    char new_path[length];
    char path[length];
    char abs_path[3 * length]; // pwd + relpath + rel_filename
    for (int i = 0; i < length * 3; i++)
        abs_path[i] = 'k';
    while (1) {
        if (get_path(path, length, stdin) != 0) {
            break;
        }
        if (access(path, F_OK | R_OK) != 0) {
            continue;
        }
        struct stat sb;
        lstat(path, &sb);
        if (S_ISREG(sb.st_mode)) {
            //printf("reg\n");
            realpath(path, new_path);
            int index = 0;
            for (int i = 0; i < length; i++) {
                if (new_path[i] == '/') {
                    index = i;
                }
                if (new_path[i] == '\0') {
                    break;
                }
            }
            int another = 0;
            char prefix[] = "link_to_";
            for (int i = 0; i < strlen(prefix); i++) {
                path[another++] = prefix[i];
            }
            for (;;) {
                path[another++] = new_path[index];
                if (new_path[index++] == '\0') {
                    break;
                }
            }
            // new_path = filename, path = symlink
            symlink(new_path, path);
        } else if (S_ISLNK(sb.st_mode)) {
            //readlink(path, new_path, length);
            //printf("not reg\n");
            for (int i = 0; i < length; i++) {
                new_path[i] = '\0';
            }
            readlink(path, new_path, length);
            if (new_path[0] == '/') {
                printf("%s\n", new_path);
                continue;
            }
            getcwd(abs_path, length);
            int index = 0;
            for (int i = 0; i < length; i++) {
                if (abs_path[i] == '\0') {
                    index = i;
                    break;
                }
            }
            //print(abs_path, 3 * length);
            abs_path[index++] = '/';
            for (int i = 0; i < length; i++) {
                if (path[i] == '\0')
                    break;
                abs_path[index] = path[i];
                index++;
            }
            //print(abs_path, 3 * length);
            while (abs_path[index - 1] != '/') {
                abs_path[--index] = '\0';
            }
            for (int i = 0; i < length; i++) {
                abs_path[index] = new_path[i];
                index++;
            }
            //print(abs_path, 3 * length);
            abs_path[index] = '\0';
            //print(abs_path, 3 * length);
            //normalize_path(abs_path);
            realpath(path, abs_path);
            printf("%s\n", abs_path);
        }
    }
    return 0;
}
