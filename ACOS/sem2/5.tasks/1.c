#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static void* count_sum(void* info) {
    int* sum = malloc(sizeof(int));
    int value = 0;
    while (scanf("%d", &value) != EOF) {
        *sum += value;
    }
    return (void*)sum;
}

int main(int argc, char** argv) {
    int N = atoi(argv[1]);
    pthread_t pthreads[N];
    for (int i = 0; i < N; i++) {
        int res = pthread_create(&pthreads[i], NULL, &count_sum, NULL);
        if (res != 0) {
            perror("pthread_create");
        } 
    }
    int sum = 0;
    for (int i = 0; i < N; i++) {
        int* value;
        int res = pthread_join(pthreads[i], (void**)&value);
        if (res != 0) {
            perror("pthread_create");
        }
        sum += *value;
        free(value);
    }   
    printf("%d", sum);
}
