#include <pthread.h>
#include <stdio.h>

static void* recursive_output(void* info) {
    int value = 0;
    if (scanf("%d", &value) == EOF) {
        return NULL;
    }
    pthread_t pthread;
    int res = pthread_create(&pthread, NULL, &recursive_output, NULL);
    if (res == -1) {
        perror("pthread_create");
        return NULL;
    }
    pthread_join(pthread, NULL);    
    printf("%d ", value);
}

int main() {
    recursive_output(NULL); 
}
