#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>

int convert_to_chars(int value, char* arr, int arr_size) {
    int copy = (value < 0 ? -value : value);
    int index = arr_size - 1;
    while (copy > 0) {
        arr[index] = '0' + (copy % 10);
        copy /= 10;
        index--;
    }
    if (value < 0) {
        arr[index] = '-';
        index--;
    }
    if (index == arr_size - 1) {
        arr[index] = '0';
        index--;
    }
    return index + 1;
}

void merge(int* v, int* buf, int l, int m, int r) {
    int ptrl = l;
    int ptrr = m;
    int ptr = l;
    while (ptrl < m || ptrr < r) {
        if (ptrl == m || (ptrl < m && ptrr < r && v[ptrr] < v[ptrl])) {
            buf[ptr] = v[ptrr];
            ptrr++;
            ptr++;
            continue;
        }
        if (ptrr == r || (ptrl < m && ptrr < r && v[ptrr] >= v[ptrl])) {
            buf[ptr] = v[ptrl];
            ptrl++;
            ptr++;
            continue;
        }
    }
    for (int i = l; i < r; i++) {
        v[i] = buf[i];
    }
}

void mergesort(int* v, int* buf, int l, int r) {
    if (l >= r - 1) {
        return;
    }
    int m = (l + r) / 2;
    mergesort(v, buf, l, m);
    mergesort(v, buf, m, r);
    merge(v, buf, l, m, r);
}

int wopen(int num) {
    const int max_len = 5;
    char x[] = "_____temp";
    int index = convert_to_chars(num, x, max_len);
    return open((const char*)(x + index), O_WRONLY | O_TRUNC | O_CREAT, 0666);
}

int ropen(int num) {
    const int max_len = 5;
    char x[] = "_____temp";
    int index = convert_to_chars(num, x, max_len);
    return open((const char*)(x + index), O_RDONLY);
}

int main(int argc, char* argv[])
{
    int fd1 = open(argv[1], O_RDONLY);
    const int bucket_size = 3;
    int bucket[bucket_size];
    const int buffer_size = bucket_size;
    int buffer[buffer_size];
    int just_read = 0;
    int files = 0;  
    while (just_read = read(fd1, bucket, sizeof(int) * bucket_size)) { 
        int real_bucket_size = just_read / sizeof(int);
        mergesort(bucket, buffer, 0, real_bucket_size); 
        int fd2 = wopen(files);    
        write(fd2, bucket, real_bucket_size * sizeof(int));
        close(fd2);
        files++;
    }
    close(fd1);
 
    const int mini_bucket_size = 2;
    int elements[files][mini_bucket_size];
    int elements_size[files];
    int elements_ptr[files];
    int elements_read[files];
    for (int i = 0; i < files; i++) {
        elements_read[i] = 0;
        elements_ptr[i] = 0;
        int fd = ropen(i);
        int read_bytes = read(fd, elements[i], mini_bucket_size * sizeof(int));
        elements_size[i] = read_bytes / sizeof(int);
        close(fd);
    }
    int fd_out = open(argv[1], O_WRONLY | O_TRUNC | O_CREAT, 0666);
    int buffer_ptr = 0;
    while (1) {
        int id = -1;
        for (int i = 0; i < files; i++) {
            if (elements_ptr[i] < elements_size[i] && (id == -1 || elements[id][elements_ptr[id]] >= elements[i][elements_ptr[i]])) {
                id = i;
            }
        }
        if (id == -1) {
            write(fd_out, buffer, buffer_ptr * sizeof(int)); 
            break;
        }
        if (buffer_ptr == buffer_size) {
            write(fd_out, buffer, buffer_ptr * sizeof(int));
            buffer_ptr = 0;
        }
        buffer[buffer_ptr] = elements[id][elements_ptr[id]];
        elements_ptr[id]++;
        buffer_ptr++;
        if (elements_ptr[id] == elements_size[id]) {
            elements_read[id] += elements_size[id];
            elements_ptr[id] = 0;
            int fd = ropen(id);
            lseek(fd, elements_read[id] * sizeof(int), SEEK_SET);
            int read_bytes = read(fd, elements[id], mini_bucket_size * sizeof(int));
            elements_size[id] = read_bytes / sizeof(int);
            close(fd);
        }
    }
    close(fd_out);
    return 0;
}
