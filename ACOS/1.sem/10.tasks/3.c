#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>

int convert_to_chars(int value, char* arr, int arr_size)
{
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

void fill_matrix(char* data, int size, int iter, const int W, const int Len) {
    if (size == 0) {
        return;
    }
    if (size == 1) {
        convert_to_chars(iter, data, W);
        return;
    }
    for (int i = 0; i < size - 1; i++) {
        convert_to_chars(iter, data, W);
        data += W;
        iter++;
    }
    for (int i = 0; i < size - 1; i++) {
        convert_to_chars(iter, data, W);
        data += Len;
        iter++;
    }
    for (int i = 0; i < size - 1; i++) {
        convert_to_chars(iter, data, W);
        data -= W;
        iter++;
    }
    for (int i = 0; i < size - 1; i++) {
        convert_to_chars(iter, data, W);
        data -= Len;
        iter++;
    }
    data += Len;
    data += W;
    fill_matrix(data, size - 2, iter, W, Len);
    return;
}

int main(int argc, char* argv[])
{
    int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    int fd1;
    if ((fd1 = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, mode)) < 0) {
        printf ("can't create %s for writing", argv[1]);
        return 0;
    }
    int N = atoi(argv[2]), W = atoi(argv[3]);
    int StringLen = (N * W + 1);
    int size = N * StringLen;
    if (fallocate(fd1, 0, 0, size) != 0) {
        printf("error with allocating\n");
        return 0;
    } 
    char* data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0); 
    if (data == (caddr_t)(-1)) {
        printf("error mmap\n");
        return 0;
    } 
    for (int i = 0; i < size; i++) {
        data[i] = ' ';
    }
    for (int i = 0; i < N; i++) {
        data[i * StringLen + N * W] = '\n';
    }
    fill_matrix(data, N, 1, W, StringLen);
    munmap(data, size);
    close(fd1);
}
