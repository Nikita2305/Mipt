#define FUSE_USE_VERSION 30 // API version 3.0
#include <fuse3/fuse.h>
#include <fuse3/fuse_common.h>
#include <fuse3/fuse_lowlevel.h>
#include <fuse3/fuse_opt.h>

#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <linux/limits.h>

#include <errno.h>

typedef struct {
    char name[NAME_MAX];
    int size;
    int offset;
    char* data;
} File;

struct {
    int size;
    int count;
    char* mem;

    File* files;
} fff;

// contents of file
static const char DummyData[] = "Hello, World!\n";

// callback function to be called after 'stat' system call
int my_stat(const char* path, struct stat* st, struct fuse_file_info* fi)
{
    // check if accessing root directory
    if (0 == strcmp("/", path)) {
        st->st_mode = 0555 | S_IFDIR; // file type - dir, access read only
        st->st_nlink = 2;             // at least 2 links: '.' and parent
        return 0;                     // success!
    }
    int res = 0;
    for (int i = 0; i < fff.count; ++i) {
        if (strcmp(path, fff.files[i].name) == 0) {
            res = i + 1;
        }
    }
    if (!res) {
        return -ENOENT; // error: we have no files other than a.txt and b.txt
    }

    st->st_mode = S_IFREG | 0444; // file type - regular, access read only
    st->st_nlink = 1;             // one link to file
    st->st_size = fff.files[res - 1].size; // bytes available
    return 0;                              // success!
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
    if (0 != strcmp(path, "/")) {
        return -ENOENT; // we do not have subdirectories
    }

    // two mandatory entries: the directory itself and its parent
    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);

    for (int i = 0; i < fff.count; ++i) {
        filler(out, fff.files[i].name + 1, NULL, 0, 0);
    }
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
        if (strcmp(path, fff.files[i].name) == 0) {
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
    int res = 0;
    for (int i = 0; i < fff.count; ++i) {
        if (strcmp(path, fff.files[i].name) == 0) {
            res = i;
        }
    }
    int i = res;
    if (off > fff.files[i].size)
        return 0;
    if (off + size > fff.files[i].size)
        size = fff.files[i].size - off;
    const void* data = fff.files[i].data + off;
    // copy contents into the buffer to be filled by 'read' system call
    memcpy(out, data, size);
    // return value is bytes count (0 or positive) or an error (negative)
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
        printf("%s %d\n", fff.files[i].name, fff.files[i].size);
        fff.files[i].offset = offset;
        fff.files[i].data = ptr + offset;
        offset += fff.files[i].size;
    }
}

int main(int argc, char* argv[])
{
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
