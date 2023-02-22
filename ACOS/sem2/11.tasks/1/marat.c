#define FUSE_USE_VERSION 30

#include <dirent.h>
#include <errno.h>

#include <fuse3/fuse.h>
#include <fuse3/fuse_common.h>
#include <fuse3/fuse_lowlevel.h>
#include <fuse3/fuse_opt.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct {
    char** buffer;
    size_t capacity;
    size_t size;
} strset_t;

void strset_init(strset_t* strset)
{
    const size_t INITIAL_CAPACITY = 1;

    strset->capacity = INITIAL_CAPACITY;
    strset->size = 0;
    strset->buffer = malloc(strset->capacity * sizeof(char*));
}

void strset_destroy(strset_t* strset)
{
    free(strset->buffer);
}

char* strset_get(strset_t* strset, size_t index)
{
    return strset->buffer[index];
}

void strset_insert(strset_t* strset, char* value)
{
    for (size_t i = 0; i < strset->size; ++i) {
        if (strcmp(value, strset->buffer[i]) == 0) {
            return;
        }
    }

    if (strset->size == strset->capacity) {
        strset->capacity *= 2;
        char** new_buffer = malloc(strset->capacity * sizeof(char*));
        memcpy(new_buffer, strset->buffer, strset->size * sizeof(char*));
        free(strset->buffer);
        strset->buffer = new_buffer;
    }

    strset->buffer[strset->size++] = value;
}

strset_t src;

void parse_src(const char* src_arg)
{
    strset_init(&src);

    char current[PATH_MAX + 5];
    size_t pos = 0;
    size_t src_arg_len = strlen(src_arg);

    for (size_t i = 0; i < src_arg_len; ++i) {
        if (src_arg[i] == ':') {
            current[pos] = '\0';
            pos = 0;

            char* abspath = malloc(PATH_MAX + 5);
            realpath(current, abspath);

            strset_insert(&src, abspath);
        } else {
            current[pos++] = src_arg[i];
        }
    }

    current[pos] = '\0';
    char* abspath = malloc(PATH_MAX + 5);
    realpath(current, abspath);
    strset_insert(&src, abspath);
}

void free_src() {
    for (size_t i = 0; i < src.size; ++i) {
        free(strset_get(&src, i));
    }
    strset_destroy(&src);
}

typedef struct {
    strset_t dirs;
    strset_t files;
    __mode_t mode;
} mergefs_dir_t;

int mergefs_dir_get(mergefs_dir_t* mergefs_dir, const char* path)
{
    int found = 0;

    strset_init(&mergefs_dir->dirs);
    strset_init(&mergefs_dir->files);

    mergefs_dir->mode = -1;

    char path_buffer[PATH_MAX + 5];
    char entry_path_buffer[PATH_MAX + 5];

    for (size_t i = 0; i < src.size; ++i) {
        strcpy(path_buffer, strset_get(&src, i));
        strcat(path_buffer, "/");
        strcat(path_buffer, path);

        struct stat dir_stat;
        if (stat(path_buffer, &dir_stat) != 0 || !S_ISDIR(dir_stat.st_mode)) {
            continue;
        }

        mergefs_dir->mode &= dir_stat.st_mode;

        found = 1;

        DIR* dir = opendir(path_buffer);
        if (!dir) {
            continue;
        }

        struct dirent* dirent;

        while ((dirent = readdir(dir))) {
            char* name = malloc(strlen(dirent->d_name) + 1);
            strcpy(name, dirent->d_name);

            strcpy(entry_path_buffer, path_buffer);
            strcat(entry_path_buffer, "/");
            strcat(entry_path_buffer, name);

            struct stat entry_stat;
            stat(entry_path_buffer, &entry_stat);

            if (S_ISDIR(entry_stat.st_mode)) {
                strset_insert(&mergefs_dir->dirs, name);
            } else {
                strset_insert(&mergefs_dir->files, name);
            }
        }
        closedir(dir);
    }

    return found;
}

void mergefs_dir_destroy(mergefs_dir_t* mergefs_dir)
{
    strset_destroy(&mergefs_dir->dirs);
    strset_destroy(&mergefs_dir->files);
}

void mergefs_get_file_path(const char* fake_path, char* path)
{
    path[0] = '\0';

    char path_buffer[PATH_MAX + 5];
    struct timespec last_modified;

    for (size_t i = 0; i < src.size; ++i) {
        strcpy(path_buffer, strset_get(&src, i));
        strcat(path_buffer, "/");
        strcat(path_buffer, fake_path);

        struct stat file_stat;
        if (stat(path_buffer, &file_stat) != 0 || S_ISDIR(file_stat.st_mode)) {
            continue;
        }

        if (path[0] == '\0' ||
            last_modified.tv_sec < file_stat.st_mtim.tv_sec ||
            (last_modified.tv_sec == file_stat.st_mtim.tv_sec &&
             last_modified.tv_nsec < file_stat.st_mtim.tv_nsec)) {

            strcpy(path, path_buffer);
            last_modified = file_stat.st_mtim;
        }
    }
}

int fdfd;

int mergefs_fuse_readdir(
    const char* path,
    void* out,
    fuse_fill_dir_t filler,
    off_t off,
    struct fuse_file_info* fi,
    enum fuse_readdir_flags flags)
{
    // write(fdfd, "hell", 4);
    mergefs_dir_t dir;
    if (!mergefs_dir_get(&dir, path)) {
        mergefs_dir_destroy(&dir);
        return -ENOENT;
    }
    if (dir.dirs.size == 0) {
        mergefs_dir_destroy(&dir);
        return -EACCES;
    }

    for (size_t i = 0; i < dir.dirs.size; ++i) {
        dprintf(fdfd, "%s\n", strset_get(&dir.dirs, i));
        filler(out, strset_get(&dir.dirs, i), NULL, 0, 0);
        free(strset_get(&dir.dirs, i));
    }

    for (size_t i = 0; i < dir.files.size; ++i) {
        dprintf(fdfd, "%s\n", strset_get(&dir.files, i));
        // write(fdfd, "hell", 4);
        filler(out, strset_get(&dir.files, i), NULL, 0, 0);
        free(strset_get(&dir.files, i));
    }
    dprintf(fdfd, "\n");

    mergefs_dir_destroy(&dir);
    return 0;
}

int mergefs_fuse_getattr(
    const char* path,
    struct stat* st,
    struct fuse_file_info* fi)
{
    char file_path[PATH_MAX + 5];
    mergefs_dir_t dir;

    if (mergefs_dir_get(&dir, path)) {
        st->st_mode = dir.mode;
        st->st_nlink = dir.dirs.size;

        mergefs_dir_destroy(&dir);
    } else {
        mergefs_get_file_path(path, file_path);
        if (file_path[0] == '\0') {
            return -ENOENT;
        }

        struct stat real_stat;
        stat(file_path, &real_stat);

        *st = real_stat;
        st->st_nlink = 1;
    }

    return 0;
}

int mergefs_fuse_open(const char* path, struct fuse_file_info* fi)
{
    char real_path[PATH_MAX + 5];
    mergefs_get_file_path(path, real_path);

    struct stat file_stat;
    stat(real_path, &file_stat);

    if (path[0] == '\0') {
        return -ENOENT;
    }
    if (O_RDONLY != (fi->flags & O_ACCMODE) || access(real_path, R_OK) != 0) {
        return -EACCES;
    }
    return 0;
}

int mergefs_fuse_read(
    const char* path,
    char* out,
    size_t size,
    off_t off,
    struct fuse_file_info* fi)
{
    char real_path[PATH_MAX + 5];
    mergefs_get_file_path(path, real_path);

    int fd = open(real_path, O_RDONLY);

    size_t result = pread(fd, out, size, off);
    close(fd);
    return result;
}

int main(int argc, char** argv)
{
    fdfd = open("out.txt", O_RDWR | O_TRUNC | O_CREAT);
    printf("%d\n", fdfd);
    // exit(0);

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    typedef struct {
        char* src;
    } options_t;

    options_t options = {NULL};

    struct fuse_opt opt_specs[] = {
        {"--src %s", offsetof(options_t, src), 0}, {NULL, 0, 0}};

    fuse_opt_parse(&args, &options, opt_specs, NULL);

    parse_src(options.src);

    struct fuse_operations operations = {
        .readdir = mergefs_fuse_readdir,
        .getattr = mergefs_fuse_getattr,
        .open = mergefs_fuse_open,
        .read = mergefs_fuse_read};

    int ret = fuse_main(args.argc, args.argv, &operations, NULL);

    free_src();
    fuse_opt_free_args(&args);

    return ret;
}
