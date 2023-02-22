#define FUSE_USE_VERSION 30 // API version 3.0
#include <errno.h>
#include <fuse3/fuse.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct String {
    int size;
    char* data;
};

struct File {
    struct String name;
    struct String data;
};

struct FileSystem {
    int size;
    struct File* files;
};

struct FileSystem fs;

void init_fs(const char* config_path)
{
    int fd = open(config_path, O_RDONLY);
    char data[131072];
    char* ptr = data;
    int size = 0;
    while ((size = read(fd, ptr, 1024)) != 0) {
        ptr += size;
    }
    int rd = (ptr - data);
    for (int i = 1; i < rd; i++) {
        if (data[i - 1] == '\n' && data[i] == '\n') {
            ptr = data + i + 1;
        }
    }
    close(fd);
    // Now ptr is the beggining of the binary data
    fd = open(config_path, O_RDONLY);
    FILE* f = fdopen(fd, "r");
    fscanf(f, "%d", &fs.size);
    fs.files = (struct File*)malloc(fs.size * sizeof(struct File));
    for (int i = 0; i < fs.size; i++) {
        fs.files[i].name.data = malloc(128);
        fscanf(f, "%s", fs.files[i].name.data);
        fs.files[i].name.size = strlen(fs.files[i].name.data);
        fscanf(f, "%d", &fs.files[i].data.size);
        fs.files[i].data.data = malloc(fs.files[i].data.size);
        memcpy(fs.files[i].data.data, ptr, fs.files[i].data.size);
        ptr += fs.files[i].data.size;
    }
    close(fd);
}

int find_file(const char* file_path)
{
    for (int i = 0; i < fs.size; i++) {
        if (strcmp(file_path + 1, fs.files[i].name.data) == 0) {
            return i;
        }
    }
    return -1;
}

int my_stat(const char* path, struct stat* st, struct fuse_file_info* fi)
{
    if (0 == strcmp("/", path)) {
        st->st_mode = 0555 | S_IFDIR;
        st->st_nlink = 2;
        return 0;
    }

    int id = find_file(path);
    if (id == -1) {
        return -ENOENT;
    }

    st->st_mode = S_IFREG | 0444;
    st->st_nlink = 1;
    st->st_size = fs.files[id].data.size;
    return 0;
}

int my_readdir(
    const char* path,
    void* out,
    fuse_fill_dir_t filler,
    off_t off,
    struct fuse_file_info* fi,
    enum fuse_readdir_flags flags)
{
    if (0 != strcmp(path, "/")) {
        return -ENOENT;
    }

    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);

    for (int i = 0; i < fs.size; i++) {
        filler(out, fs.files[i].name.data, NULL, 0, 0);
    }

    return 0;
}

int my_open(const char* path, struct fuse_file_info* fi)
{
    if (find_file(path) == -1) {
        return -ENOENT;
    }
    if (O_RDONLY != (fi->flags & O_ACCMODE)) {
        return -EACCES;
    }
    return 0;
}

int my_read(
    const char* path,
    char* out,
    size_t size,
    off_t off,
    struct fuse_file_info* fi)
{
    int id = find_file(path);
    if (id == -1) {
        return 0;
    }
    if (off > fs.files[id].data.size)
        return 0;
    if (off + size > fs.files[id].data.size)
        size = fs.files[id].data.size - off;
    memcpy(out, fs.files[id].data.data + off, size);
    return size;
}

struct fuse_operations operations = {
    .readdir = my_readdir,
    .getattr = my_stat,
    .open = my_open,
    .read = my_read,
};

int main(int argc, char* argv[])
{
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    typedef struct {
        char* src;
        int help;
    } my_options_t;

    my_options_t my_options;
    memset(&my_options, 0, sizeof(my_options));

    struct fuse_opt opt_specs[] = {
        {"--src %s", offsetof(my_options_t, src), 0},
        {"--help", offsetof(my_options_t, help), true},
        {NULL, 0, 0}};

    fuse_opt_parse(&args, &my_options, opt_specs, NULL);

    if (my_options.help) {
        printf("%s\n", "ha-ha-ha. no help in MIPT.");
        return 0;
    }

    if (my_options.src) {
        init_fs(my_options.src);
    }

    /*printf("size: %d\n", fs.size); 
    for (int i = 0; i < fs.size; i++) {
        printf("%d %s\n", fs.files[i].name.size, fs.files[i].name.data);
        printf("%d ", fs.files[i].data.size);
        printf("\n");
    }*/
    int ret = fuse_main(args.argc, args.argv, &operations, NULL);
    return ret;
}
