#define _LARGEFILE64_SOURCE
#include <fcntl.h>
#include <zip.h>

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

zip_t* zip_arch = 0;
int out_fd;
FILE* out_file;

char path_to_zip[2 * NAME_MAX];

// callback function to be called after 'stat' system call
int my_stat(const char* path, struct stat* st, struct fuse_file_info* fi)
{
    zip_arch = zip_open(path_to_zip, ZIP_RDONLY, NULL);
    if (0 == strcmp("/", path)) {
        st->st_mode = 0555 | S_IFDIR; // file type - dir, access read only
        st->st_nlink = 2;             // at least 2 links: '.' and parent
        return 0;                     // success!
    }

    zip_int64_t num_entries = zip_get_num_entries(zip_arch, 0);

    int n = strlen(path) - 1;

    fprintf(out_file, "%d\n%s\n", num_entries, path);
    fflush(out_file);

    const char* npath = path + 1;
    for (zip_uint64_t i = 0; i < (zip_uint64_t)num_entries; i++) {
        const char* name = zip_get_name(zip_arch, i, 0);

        if (strncmp(name, npath, n) == 0) {
            if (strlen(name) == n) {
                struct zip_stat stat;
                zip_stat_index(
                    zip_arch, i, ZIP_STAT_SIZE | ZIP_STAT_MTIME, &stat);
                st->st_mode = S_IFREG | 0444;
                st->st_nlink = 1;
                st->st_size = stat.size;
                // st->st_mtim = stat.mtime;
                return 0; // success!
            } else if (strlen(name) == n + 1) {
                st->st_mode =
                    0555 | S_IFDIR; // file type - dir, access read only
                st->st_nlink = 2;   // at least 2 links: '.' and parent
                return 0;           // success!
            }
        }
    }
    return -ENOENT;
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
    zip_arch = zip_open(path_to_zip, ZIP_RDONLY, NULL);
    zip_int64_t num_entries = zip_get_num_entries(zip_arch, 0);

    out_file = fdopen(out_fd, "w");
    int n = strlen(path) - 1;

    fprintf(out_file, "%d\n%s\n", num_entries, path);
    fflush(out_file);

    const char* npath = path + 1;

    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);

    for (zip_uint64_t i = 0; i < (zip_uint64_t)num_entries; i++) {
        const char* name = zip_get_name(zip_arch, i, 0);

        if (strncmp(name, npath, n) == 0) {
            if (name[n] == '\0') {
                continue;
            }
            const char* nname = name + (n == 0 ? -1 : n) + 1;
            char* f = strstr(nname, "/");

            fprintf(out_file, "check name: {%s} with {%s}\n", nname, npath);
            fflush(out_file);

            if (nname[0] == '\0') {
                continue;
            }
            if (f == 0) {
                fprintf(out_file, "passed {%s}\n", nname);
                fflush(out_file);

                filler(out, nname, NULL, 0, 0);
            } else if (*(f + 1) == '\0') {
                *f = '\0';
                if (nname[0] == '\0') {
                    *f = '/';
                    continue;
                }
                fprintf(out_file, "passed {%s}\n", nname);
                fflush(out_file);

                filler(out, nname, NULL, 0, 0);
                *f = '/';
            }
        }
    }
    return 0;
}

// callback function to be called after 'open' system call
int my_open(const char* path, struct fuse_file_info* fi)
{
    zip_int64_t num_entries = zip_get_num_entries(zip_arch, 0);

    int n = strlen(path) - 1;

    fprintf(out_file, "%d\n%s\n", num_entries, path);
    fflush(out_file);

    const char* npath = path + 1;
    for (zip_uint64_t i = 0; i < (zip_uint64_t)num_entries; i++) {
        const char* name = zip_get_name(zip_arch, i, 0);

        if (strncmp(name, npath, n) == 0) {
            if (strlen(name) == n) {
                return 0; // success!
            } else if (strlen(name) == n + 1) {
                return 0; // success!
            }
        }
    }
    return -ENOENT;
}

int min(int a, int b)
{
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

// callback function to be called after 'read' system call
int my_read(
    const char* path,
    char* out,
    size_t size,
    off_t off,
    struct fuse_file_info* fi)
{
    zip_arch = zip_open(path_to_zip, ZIP_RDONLY, NULL);
    zip_file_t* file = zip_fopen(zip_arch, path + 1, ZIP_FL_UNCHANGED);

    const int N = 1024;
    char buffer[N];

    zip_int64_t res = 0;
    while (res = zip_fread(file, buffer, min(N, off))) {
        if (res == -1) {
            return 0;
        }
        off -= res;
    }

    size = zip_fread(file, out, size);

    zip_fclose(file);
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

    zip_arch = zip_open(src, ZIP_RDONLY, NULL);

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

    getcwd(path_to_zip, 2 * NAME_MAX);

    if (src[0] == '/') {
        strcpy(path_to_zip, src);
    } else {
        int n = strlen(path_to_zip);
        path_to_zip[n + 1] = '\0';
        path_to_zip[n] = '/';

        strcpy(path_to_zip + n + 1, src);
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
    out_fd = open("out.txt", O_RDWR | O_CREAT | O_TRUNC);
    out_file = fdopen(out_fd, "w");

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
