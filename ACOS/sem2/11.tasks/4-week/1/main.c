#define _LARGEFILE64_SOURCE
#include <fcntl.h>

#define FUSE_USE_VERSION 30 // API version 3.0
#include <fuse3/fuse.h>
#include <fuse3/fuse_common.h>
#include <fuse3/fuse_lowlevel.h>
#include <fuse3/fuse_opt.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>

#include <linux/limits.h>

#include <errno.h>

typedef struct {
    char path[NAME_MAX];
} DirP;

struct {
    int count;
    DirP* pathes;
} fff;

int out_fd;
FILE* out_file;

char cwd[NAME_MAX];

char less_time(struct timespec a, struct timespec b)
{
    return a.tv_sec < b.tv_sec ||
           (a.tv_sec == b.tv_sec && a.tv_nsec < b.tv_nsec);
}

void GetDirectory(char* buffer, const char* path, const char* dir_path)
{
    int nn = strlen(cwd);
    if (dir_path[0] == '/') {
        nn = 0;
    } else {
        strcpy(buffer, cwd);
    }
    int n = strlen(dir_path);
    strcpy(buffer + nn, dir_path);
    strcpy(buffer + n + nn, path);
    buffer[nn + n + strlen(path)] = '\0';
}

// callback function to be called after 'stat' system call
int my_stat(const char* path, struct stat* st, struct fuse_file_info* fi)
{
    // fprintf(out_file, "STAT FILE %s\n", path);
    // fflush(out_file);
    // check if accessing root directory
    if (0 == strcmp("/", path)) {
        st->st_mode = 0555 | S_IFDIR; // file type - dir, access read only
        st->st_nlink = 2;             // at least 2 links: '.' and parent
        return 0;                     // success!
    }
    for (int i = 0; i < fff.count; ++i) {
        char curdir[3 * NAME_MAX];
        GetDirectory(curdir, path, fff.pathes[i].path);

        struct stat stats;

        if (stat(curdir, &stats) != 0) {
            continue;
        }

        if (S_ISDIR(stats.st_mode)) {
            *st = stats;
            st->st_mode &= 0555 | S_IFDIR; // file type - dir, access read only
            // st->st_nlink = 2;             // at least 2 links: '.' and parent
            return 0; // success!
        }
        if (S_ISREG(stats.st_mode)) {
            *st = stats;
            st->st_mode &=
                S_IFREG | 0444; // file type - regular, access read only
            // st->st_nlink = 1;   // one link to file
            st->st_size = stats.st_size; // bytes available
            return 0;
        }
    }
    // fprintf(out_file, "ERROR MY STAT {%s}\n", path);
    return -ENOENT;
}

typedef struct list_dirs {
    char name[NAME_MAX];
    struct list_dirs* next;
} ListDirs;

ListDirs* Insert(ListDirs* last, ListDirs* cur)
{
    if (last != 0) {
        last->next = cur;
    }
    return cur;
}

// callback function to be called after 'readdir' system call
int my_readdir(
    const char* path,
    void* out,
    fuse_fill_dir_t filler,
    off_t off,
    struct fuse_file_info* fi,
    enum fuse_readdir_flags flags)
{
    ListDirs d = {.name = ".", .next = 0};
    ListDirs dd = {.name = "..", .next = 0};

    ListDirs* begin = &d;
    ListDirs* end = &d;

    end = Insert(end, &dd);

    for (int i = 0; i < fff.count; ++i) {
        char curdir[3 * NAME_MAX];
        GetDirectory(curdir, path, fff.pathes[i].path);

        out_file = fdopen(out_fd, "w");

        // fprintf(out_file, "check file: %s\n", curdir);

        DIR* dir = opendir(curdir);

        if (dir == 0) {
            continue;
        }

        // fprintf(out_file, "%d\n", dir);
        // fflush(out_file);

        while (1) {
            struct dirent* aaa = readdir(dir);
            if (!aaa)
                break;
            if (strcmp(aaa->d_name, ".") == 0)
                continue;
            if (strcmp(aaa->d_name, "..") == 0)
                continue;
            ListDirs* ptr = begin;
            int r = 1;
            while (ptr != 0) {
                if (strcmp(ptr->name, aaa->d_name) == 0) {
                    r = 0;
                }
                ptr = ptr->next;
            }
            if (r) {
                filler(out, aaa->d_name, NULL, 0, 0);
                // fprintf(out_file, "readdir: %s\n", aaa->d_name);
                // fflush(out_file);
                ListDirs* new_node = malloc(sizeof(ListDirs));
                strcpy(new_node->name, aaa->d_name);
                end = Insert(end, new_node);
            }
        }
        closedir(dir);
    }
    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);
    // fprintf(out_file, "\n\n");
    // fflush(out_file);

    /*
    if (0 != strcmp(path, "/")) {
        return -ENOENT; // we do not have subdirectories
    }

    // two mandatory entries: the directory itself and its parent
    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);

    for (int i = 0; i < fff.count; ++i) {
        filler(out, fff.files[i].name + 1, NULL, 0, 0);
    }
    */
    // // directory contents
    // filler(out, "a.txt", NULL, 0, 0);
    // filler(out, "b.txt", NULL, 0, 0);

    return 0; // success
}

// callback function to be called after 'open' system call
int my_open(const char* path, struct fuse_file_info* fi)
{
    int res = 0;
    for (int i = 0; i < fff.count; ++i) {
        char curdir[3 * NAME_MAX];
        int nn = strlen(cwd);
        if (fff.pathes[i].path[0] == '/') {
            nn = 0;
        } else {
            strcpy(curdir, cwd);
        }
        int n = strlen(fff.pathes[i].path);
        strcpy(curdir + nn, fff.pathes[i].path);
        strcpy(curdir + n + nn, path);
        curdir[nn + n + strlen(path)] = '\0';

        struct stat stats;

        if (stat(curdir, &stats) == 0) {
            res = 1;
        }
    }

    if (!res) {
        return -ENOENT; // we have only two files in out filesystem
    }

    if (O_RDONLY != (fi->flags & O_ACCMODE)) {
        return -EACCES; // file system is read-only, so can't write
    }

    return 0; // success!
}

// callback function to be called after 'read' system call
int my_read(
    const char* path,
    char* out,
    size_t size,
    off_t off,
    struct fuse_file_info* fi)
{
    int res = -1;
    struct timespec res_time;
    for (int i = 0; i < fff.count; ++i) {
        char curdir[3 * NAME_MAX];
        GetDirectory(curdir, path, fff.pathes[i].path);

        struct stat stats;

        if (stat(curdir, &stats) == 0) {
            struct timespec cur_time = stats.st_mtim;
            if (res == -1 || less_time(res_time, cur_time)) {
                res = i;
                res_time = cur_time;
            }
        }
    }
    /*
    for (int i = 0; i < fff.count; ++i) {
        if (strcmp(path, fff.files[i].name) == 0) {
            res = i;
        }
    }
    */
    int i = res;
    char curdir[3 * NAME_MAX];
    GetDirectory(curdir, path, fff.pathes[i].path);
    /*
    int nn = strlen(cwd);
    if (fff.pathes[i].path[0] == '/') {
        nn = 0;
    } else {
        strcpy(curdir, cwd);
    }
    int n = strlen(fff.pathes[i].path);
    strcpy(curdir + nn, fff.pathes[i].path);
    strcpy(curdir + n + nn, path);
    curdir[nn + n + strlen(path)] = '\0';
    */

    struct stat stats;

    stat(curdir, &stats) == 0;

    int size_file = stats.st_size;

    if (off > size_file)
        return 0;
    if (off + size > size_file)
        size = size_file - off;

    int fd = open(curdir, O_RDONLY);
    lseek64(fd, (off64_t)off, SEEK_SET);
    int rrrr = read(fd, out, size);

    close(fd);
    return rrrr;
    /*
    const void* data = fff.files[i].data + off;
    // copy contents into the buffer to be filled by 'read' system call
    memcpy(out, data, size);
    // return value is bytes count (0 or positive) or an error (negative)
    // */
    return size;
}

// register functions as callbacks
struct fuse_operations operations = {
    .readdir = my_readdir,
    .getattr = my_stat,
    .open = my_open,
    .read = my_read,
};

void open_filesystem(const char* src)
{
    fff.count = 1;
    int i = 0;
    while (src[i] != '\0') {
        fff.count += src[i] == ':';
        ++i;
    }

    fff.pathes = malloc(sizeof(DirP) * fff.count);

    const char* cur = src;
    for (int i = 0; i < fff.count; ++i) {
        const char* end = cur;
        while (*end != ':' && *end != '\0')
            end++;

        memcpy(fff.pathes[i].path, cur, end - cur);
        fff.pathes[i].path[end - cur] = '\0';
        cur = end + 1;
    }
    /*
    int fd = open(src, O_RDONLY);

    struct stat s;
    fstat(fd, &s);

    fff.size = s.st_size;
    fff.mem = mmap(0, fff.size, PROT_READ, MAP_PRIVATE, fd, 0);

    FILE* file = fdopen(fd, "r");

    fscanf(file, "%d", &fff.count);

    fff.files = malloc(sizeof(File) * fff.count);

    char* ptr = strstr(fff.mem, "\n\n") + 2;
    // write(1, ptr, fff.size - (fff.mem - ptr));
    int offset = 0;
    for (int i = 0; i < fff.count; ++i) {
        fff.files[i].name[0] = '/';
        fscanf(file, "%s %d", fff.files[i].name + 1, &fff.files[i].size);
        fff.files[i].offset = offset;
        fff.files[i].data = ptr + offset;
        offset += fff.files[i].size;
    }
    */
}

int main(int argc, char* argv[])
{
    {
        getcwd(cwd, NAME_MAX);
        int n = strlen(cwd);
        cwd[n] = '/';
        cwd[n + 1] = '\0';
    }

    out_fd = open("out.txt", O_RDWR | O_CREAT | O_TRUNC);
    // out_file = fdopen(out_fd, "w");

    // initialize modificable array {argc, argv}
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    // struct to be filled by options parsing
    typedef struct {
        char* src;
        int help;
    } my_options_t;

    my_options_t my_options;
    memset(&my_options, 0, sizeof(my_options));

    // options specifications
    struct fuse_opt opt_specs[] = {
        // pattern: match --src then string
        // the string value to be written to my_options_t.src
        {"--src %s", offsetof(my_options_t, src), 0},
        // pattern: match --help
        // if found, 'true' value to be written to my_options_t.help
        {"--help", offsetof(my_options_t, help), true},
        // end-of-array: all zeroes value
        {NULL, 0, 0}};

    // parse command line arguments, store matched by 'opt_specs'
    // options to 'my_options' value and remove them from {argc, argv}
    fuse_opt_parse(&args, &my_options, opt_specs, NULL);

    // if (my_options.help) {
    //     show_help_and_exit();
    // }

    if (my_options.src) {
        open_filesystem(my_options.src);
    }

    // pass rest options but excluding --src and --help to mount.fuse3
    int ret = fuse_main(args.argc, args.argv, &operations, NULL);

    return ret;
}
