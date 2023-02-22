#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

struct config {
    int index;
    pthread_mutex_t* mutexes;
    double* array;
    int size;
    int iter;
};

void change_value(double* value, double diff, pthread_mutex_t* mutex) {
    pthread_mutex_lock(mutex);
    *value += diff;   
    pthread_mutex_unlock(mutex);
}

void* thread_routine(void* info) {
    struct config* config = (struct config*)info;
    int prev = (config->index == 0 ? config->size - 1 : config->index - 1);
    int next = (config->index == config->size - 1 ? 0 : config->index + 1);
    for (int i = 0; i < config->iter; i++) {
        change_value(&config->array[config->index], 1, &config->mutexes[config->index]);
        change_value(&config->array[prev], 0.99, &config->mutexes[prev]);
        change_value(&config->array[next], 1.01, &config->mutexes[next]);
    }
    return NULL;
} 

int main(int argc, char* argv[]) {
    int N = atoi(argv[1]);
    int k = atoi(argv[2]);
    pthread_mutex_t mutexes[k];
    double array[k];
    struct config configs[k];
    pthread_t threads[k];
    for (int i = 0; i < k; i++) {
        array[i] = 0;
        if (0 != pthread_mutex_init(&mutexes[i], NULL)) {
            perror("mutex_init");
        }
        configs[i].index = i;
        configs[i].mutexes = mutexes;
        configs[i].array = array;
        configs[i].size = k;
        configs[i].iter = N;
    }
    for (int i = 0; i < k; i++) {
        if (0 != pthread_create(&threads[i], NULL, &thread_routine, &configs[i])) {
            perror("pthread_create");
        }
    }
    for (int i = 0; i < k; i++) {
        if (0 != pthread_join(threads[i], NULL)) {
            perror("pthread_join");
        }
    }
    for (int i = 0; i < k; i++) {
        if (0 != pthread_mutex_destroy(&mutexes[i])) {
            perror("mutex_destroy");
        }
    }
    for (int i = 0; i < k; i++) {
        printf("%.10g ", array[i]);
    }
}
