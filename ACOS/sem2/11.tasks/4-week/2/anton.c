#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <zip.h>

#define FUSE_USE_VERSION 30
#include <fuse3/fuse.h>
#include <fuse3/fuse_common.h>
#include <fuse3/fuse_lowlevel.h>
#include <fuse3/fuse_opt.h>
#undef FUSE_USE_VERSION

int const buf_size = 1 << 20;
int const name_len = 1 << 12;
typedef struct {
    char buf[/*buf_size=*/1 << 20];
    char name[/*name_len=*/1 << 12];
    //char archive[/*name_len=*/1 << 12];
    zip_t* archive;
    int logfd;
} data_t;

data_t* data()
{
    static data_t data;
    return &data;
}

void open_filesystem(const char* image_filename)
{
    //data()->archive[0] = '\0';
    //strcat(data()->archive, image_filename);
    data()->archive = zip_open(image_filename, ZIP_RDONLY, NULL);
}

char* get_path_by_id(int id)
{
    zip_t* archive =
        data()->archive; //zip_open(data()->archive, ZIP_RDONLY, NULL);
    char* name = data()->name;
    name[0] = '/';
    name[1] = '\0';
    strcat(name, zip_get_name(archive, id, 0));
    return name;
}

char* get_name(const char* path)
{
    zip_t* archive =
        data()->archive; //zip_open(data()->archive, ZIP_RDONLY, NULL);
    if (0 == strcmp(path, "/")) {
        char* name = data()->name;
        name[0] = '/';
        name[1] = '\0';
        return name;
    }
    zip_int64_t num_entries = zip_get_num_entries(archive, 0);
    for (zip_int64_t i = 0; i < num_entries; ++i) {
        char* name = get_path_by_id(i);
        if (0 != strcmp(name, path)) {
            int namelen = strlen(name);
            if (strlen(path) + 1 != namelen || name[namelen - 1] != '/' ||
                0 != strncmp(path, name, namelen - 1))
                continue;
        }
        return name;
    }
    return NULL;
}

int my_open(const char* path, struct fuse_file_info* fi)
{
    dprintf(data()->logfd, "open %s\n", path);
    if (0 == strcmp(path, "/")) {
        if (O_RDONLY != (fi->flags & O_ACCMODE))
            return -EACCES;
        return 0;
    }
    const char* name = get_name(path);
    dprintf(data()->logfd, "%s %s\n", path, name);
    if (!name)
        return -ENOENT;
    if (O_RDONLY != (fi->flags & O_ACCMODE))
        return -EACCES;
    return 0;
}

int my_read(
    const char* path,
    char* out,
    size_t size,
    off_t off,
    struct fuse_file_info* fi)
{
    zip_t* archive =
        data()->archive; //zip_open(data()->archive, ZIP_RDONLY, NULL);
    dprintf(data()->logfd, "read %s\n", path);
    if (strlen(path) == 0 || path[0] != '/')
        return -ENOENT;

    if (0 == strcmp(path, "/"))
        return -EACCES;

    const char* name = get_name(path);
    if (name[strlen(name) - 1] == '/')
        return -EACCES;

    zip_file_t* file = zip_fopen(archive, name + 1, 0);
    if (file == NULL) {
        return -EACCES;
    }

    char* buf = data()->buf;
    zip_int64_t was_read = zip_fread(file, buf, off + size);
    if (was_read < 0)
        return -EACCES;

    if (was_read <= off)
        return 0;
    size = was_read - off;
    memcpy(out, buf + off, size);
    return size;
}

int my_stat(const char* path, struct stat* st, struct fuse_file_info* fi)
{
    zip_t* archive =
        data()->archive; //zip_open(data()->archive, ZIP_RDONLY, NULL);

    dprintf(data()->logfd, "stat %s\n", path);
    if (strlen(path) == 0 || path[0] != '/')
        return -ENOENT;

    if (0 == strcmp(path, "/")) {
        st->st_mode = (0555 | S_IFDIR);
        st->st_nlink = 2;
        return 0;
    }

    const char* name = get_name(path);
    dprintf(data()->logfd, "name: %s\n", name);
    if (!name)
        return -ENOENT;
    if (name[strlen(name) - 1] == '/') {
        st->st_mode = (0555 | S_IFDIR);
        st->st_nlink = 2;
        dprintf(data()->logfd, "good stat\n");
        return 0;
    }

    struct zip_stat zst;
    if (zip_stat(archive, name + 1, ZIP_STAT_SIZE, &zst) != 0) {
        dprintf(data()->logfd, "bad stat\n");
        return -EACCES;
    }

    st->st_mode = (0444 | S_IFREG);
    st->st_nlink = 1;
    st->st_size = zst.size;
    dprintf(data()->logfd, "good stat, %lu\n", zst.size);
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
    zip_t* archive =
        data()->archive; //zip_open(data()->archive, ZIP_RDONLY, NULL);
    dprintf(data()->logfd, "readdir %s \n", path);

    if (strlen(path) == 0 || path[0] != '/')
        return -ENOENT;

    int open_flags = my_open(path, fi);
    if (open_flags != 0)
        return open_flags;

    char* dirname = data()->buf;
    dirname[0] = '\0';
    char* tmp = get_name(path);
    strcat(dirname, tmp);
    size_t dirlen = strlen(dirname);

    dprintf(data()->logfd, "%s\n", dirname);

    if (dirname[strlen(dirname) - 1] != '/')
        return -EACCES;

    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);

    zip_int64_t num_entries = zip_get_num_entries(archive, 0);

    for (zip_int64_t i = 0; i < num_entries; i++) {
        char* name = get_path_by_id(i);
        dprintf(data()->logfd, "%s\n", name);
        if (0 != strncmp(name, dirname, dirlen))
            continue;
        if (strlen(name) == dirlen)
            continue;
        char* next_dir = strstr(name + dirlen, "/");
        if (next_dir == NULL || *(next_dir + 1) == '\0') {
            if (next_dir)
                *next_dir = '\0';
            filler(out, name + dirlen, NULL, 0, 0);
        }
    }
    return 0;
}

typedef struct {
    char* src;
} my_options_t;

int main(int argc, char* argv[])
{
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    my_options_t my_options;
    memset(&my_options, 0, sizeof(my_options));

    struct fuse_opt opt_specs[] = {{"--src %s", 0, 0}, {NULL, 0, 0}};

    fuse_opt_parse(&args, &my_options, opt_specs, NULL);
    data()->logfd = open("log.txt", O_TRUNC | O_RDWR);

    if (my_options.src) {
        open_filesystem(my_options.src);
    } else {
        exit(1);
    }

    struct fuse_operations operations = {
        .readdir = my_readdir,
        .getattr = my_stat,
        .open = my_open,
        .read = my_read,
    };

    int ret = fuse_main(args.argc, args.argv, &operations, NULL);

    return ret;
}
