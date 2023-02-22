#define FUSE_USE_VERSION 30   // API version 3.0
#include <fuse3/fuse.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>

int DEBUG = 0;

void str(const char* s) {
    if (!DEBUG) {
        return;
    }
    printf("LOG: %s\n", s);
}

void in(int d) {
    if (!DEBUG) {
        return;
    }
    printf("LOG: %d\n", d);
}

struct FileSystem {
    int size;
    char** dirs;
} fs; 

// ok
void init_fs(const char* paths) {
    int length = strlen(paths);
    char* temp = malloc(length + 2); // destroyed
    strcpy(temp, paths);
    temp[length] = ':';
    temp[++length] = '\0';

    fs.size = 0;
    for (int i = 0; i < length; i++) {
        fs.size += (temp[i] == ':');
    }
    fs.dirs = malloc(fs.size * sizeof(char*)); // destroyed

    int prev = -1;
    for (int i = 0; i < fs.size; i++) {
        int id = strstr(temp + prev + 1, ":") - temp;
        temp[id] = '\0';
        fs.dirs[i] = realpath(temp + prev + 1, NULL); // destroyed
        temp[id] = ':';
        prev = id;
    }
    free(temp);
}

void destroy_fs() {
    for (int i = 0; i < fs.size; i++) {
        free(fs.dirs[i]);
    }
    free(fs.dirs);
}

struct Strings {
    char** data;
    int size; 
};

// ok
void init_strings(struct Strings* strings) {
    strings->data = NULL;
    strings->size = 0;
}

// ok
void append_and_destroy(struct Strings* reciever, struct Strings* sender) {
    if (sender->size == 0) {
        return;
    }
    int flags[sender->size];
    int count = 0;
    for (int i = 0; i < sender->size; i++) {
        flags[i] = 1;
        for (int j = 0; j < reciever->size; j++) {
            if (strcmp(sender->data[i], reciever->data[j]) == 0) {
                flags[i] = 0;
                break;
            }
        }
        count += flags[i];
    }
    reciever->data = realloc(reciever->data, sizeof(char*) * (reciever->size + count));
    for (int i = 0; i < sender->size; i++) {
        if (flags[i]) {
            reciever->data[reciever->size++] = sender->data[i];
        } else {
            free(sender->data[i]);
        }
    }
    free(sender->data);
    sender->data = NULL;
    sender->size = 0;
}

struct FSObject {
    int is_dir;
    char* path;
    __mode_t mode;    
    struct Strings related_dirs;
    struct Strings related_files;   
};

// ok
int fill_candidates(struct FSObject* arr, const char* relpath) {
    int ptr = 0;
    for (int i = 0; i < fs.size; i++) {
        struct FSObject obj;
        // path
        obj.path = malloc(2 * PATH_MAX + 100);
        strcpy(obj.path, fs.dirs[i]);
        strcat(obj.path, "/");
        strcat(obj.path, relpath);
     
        // mode
        struct stat st1;
        if (-1 == stat(obj.path, &st1)) {
            free(obj.path);
            continue;
        }
        obj.mode = st1.st_mode;
 
        // is_dir
        if (S_ISDIR(st1.st_mode)) {
            obj.is_dir = 1;
        } else {
            obj.is_dir = 0;
        }

        // related files/dirs
        init_strings(&obj.related_dirs);
        init_strings(&obj.related_files);
        if (!obj.is_dir) {
            arr[ptr++] = obj;
            continue;     
        }

        DIR* dir = opendir(obj.path);
        struct dirent* dirent;
        char buffer[2 * PATH_MAX + 100];
        while ((dirent = readdir(dir))) {
            struct Strings s;
            s.size = 1;
            s.data = malloc(1 * sizeof(char*)); // destroyed 
            s.data[0] = malloc(strlen(dirent->d_name) + 1);  // moved       
            strcpy(s.data[0], dirent->d_name);

            strcpy(buffer, obj.path);
            strcat(buffer, "/");
            strcat(buffer, s.data[0]);
            struct stat st;
            stat(buffer, &st);

            if (S_ISDIR(st.st_mode)) {
                append_and_destroy(&obj.related_dirs, &s);
            } else {
                append_and_destroy(&obj.related_files, &s);
            }
        }
        closedir(dir);
        arr[ptr++] = obj;
    }
    return ptr;
}

// ok
int choose_file(struct FSObject* arr, int size) {
    int ret = -1;
    struct timespec last;
    for (int i = 0; i < size; i++) {  
        if (arr[i].is_dir) {
            continue;
        }
        struct stat st;
        if (stat(arr[i].path, &st) != 0) {
            continue;
        } 
        if (ret == -1 || last.tv_sec < st.st_mtim.tv_sec ||
                (last.tv_sec == st.st_mtim.tv_sec && last.tv_nsec < st.st_mtim.tv_nsec)) {
            last = st.st_mtim;
            ret = i;
        }   
    }
    return ret;
}

void destroy_strings(struct Strings* s) {
    if (s->size == 0 && s->data == NULL) {
        return;
    }
    if (s->size == 0 || s->data == NULL) {
       assert(0); 
    }
    for (int i = 0; i < s->size; i++) {
        if (s->data[i] == NULL) {
            assert(0);
        }
        free(s->data[i]);
    }
    free(s->data);
}

void destroy_FSObject(struct FSObject* obj) {
    if (obj->path != NULL) {
        free(obj->path);
    }
    destroy_strings(&obj->related_dirs);
    destroy_strings(&obj->related_files);
}

void destroy_candidates_except_id(struct FSObject* arr, int size, int id) {
    for (int i = 0; i < size; i++) {
        if (i == id) {
            continue;
        }
        destroy_FSObject(&arr[i]);
    }
}

int Fill(struct FSObject* obj, const char* relpath, int is_dir) {
    struct FSObject candidates[fs.size];
    int n = fill_candidates(candidates, relpath);
    if (n == 0) {
        return -1;
    }
    obj->path = NULL;
    int found = 0;
    for (int i = 0; i < n; i++) {
        if (candidates[i].is_dir == is_dir) {
            found = 1;
        }
    }
    if (!found) {
        destroy_candidates_except_id(candidates, n, -1);
        return -1;
    }
    obj->is_dir = is_dir;
    if (!is_dir) {
        int id = choose_file(candidates, n);
        if (id == -1) {
            destroy_candidates_except_id(candidates, n, -1);
            return -1;
        }
        destroy_candidates_except_id(candidates, n, id);
        *obj = candidates[id];
        return 0;
    }
    obj->mode = -1;
    init_strings(&obj->related_dirs);
    init_strings(&obj->related_files);
    for (int i = 0; i < n; i++) {
        if (candidates[i].is_dir) {
            obj->mode &= candidates[i].mode;
            append_and_destroy(&obj->related_dirs, &candidates[i].related_dirs);
            append_and_destroy(&obj->related_files, &candidates[i].related_files);
        }
    }
    destroy_candidates_except_id(candidates, n, -1);
    return 0;
}

int my_stat(const char *path, struct stat *st, struct fuse_file_info *fi) {

    struct FSObject obj;
    if (0 == Fill(&obj, path, 1)) {
        st->st_mode = obj.mode;
        st->st_nlink = obj.related_dirs.size;
        in(st->st_mode);
        st->st_mode &= 0777555;
        in(st->st_mode);
        destroy_FSObject(&obj);
        return 0;
    } else if (0 == Fill(&obj, path, 0)) {
        struct stat st1;
        stat(obj.path, &st1);
        *st = st1;
        st->st_nlink = 1;
        in(st->st_mode);
        st->st_mode &= 0777444;
        in(st->st_mode);
        destroy_FSObject(&obj);
        return 0;    
    }
    return -ENOENT;
    
    
}

int my_readdir(const char *path, void *out, fuse_fill_dir_t filler, off_t off,
               struct fuse_file_info *fi, enum fuse_readdir_flags flags) {

    struct FSObject obj;
    if (0 != Fill(&obj, path, 1)) {
        return -ENOENT;
    }
    for (int i = 0; i < obj.related_dirs.size; i++) {
        str(obj.related_dirs.data[i]);
        filler(out, obj.related_dirs.data[i], NULL, 0, 0);
    } 
    str("");
    for (int i = 0; i < obj.related_files.size; i++) {
        str(obj.related_files.data[i]);
        filler(out, obj.related_files.data[i], NULL, 0, 0);
    }
    destroy_FSObject(&obj);
    return 0;
}

int my_open(const char *path, struct fuse_file_info *fi)
{
    
    struct FSObject obj;
    if (0 != Fill(&obj, path, 0)) {
        return -ENOENT;
    }
    if (O_RDONLY != (fi->flags & O_ACCMODE)) {
        destroy_FSObject(&obj);
        return -EACCES;
    }
    destroy_FSObject(&obj);
    return 0;
}

int my_read(const char *path, char *out, size_t size, off_t off, struct fuse_file_info *fi) {
    struct FSObject obj;
    if (0 != Fill(&obj, path, 0)) {
        return 0;
    }
    int fd = open(obj.path, O_RDONLY);
    int just_read = pread(fd, out, size, off);
    close(fd);
    destroy_FSObject(&obj);
    return just_read;
}

struct fuse_operations operations = {
    .readdir = my_readdir,
    .getattr = my_stat,
    .open    = my_open,
    .read    = my_read,
};

int main(int argc, char *argv[]) {
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
   
    typedef struct {
        char *src;
        int  help;
    } my_options_t;

    my_options_t my_options;
    memset(&my_options, 0, sizeof(my_options));

    struct fuse_opt opt_specs[] = {
        { "--src %s", offsetof(my_options_t, src)   , 0     },
        { "--help"  , offsetof(my_options_t, help)  , true  },
        { NULL      , 0                             , 0     }
    };
	
    fuse_opt_parse(&args, &my_options, opt_specs, NULL);

    if (my_options.help) {
        printf("%s\n", "ha-ha-ha. no help in MIPT.");
        return 0;
    }

    if (my_options.src) {
        init_fs(my_options.src);

        int ret = fuse_main(
            args.argc, args.argv, 
            &operations,    
            NULL
        );

        // destroy_fs(); // Only with -d

        return ret;
    } else {
        printf("%s\n", "ERROR! --src not fount");
        return 1;
    }
}
