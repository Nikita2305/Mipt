#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct config {
    long long A;
    long long B;
    pthread_cond_t* waiter;
    pthread_mutex_t* mutex;
    long long* value;
};

int isprime(long long x)
{
    if (x == 1) {
        return 0;
    }
    for (long long i = 2; i * i <= x; ++i) {
        if (x % i == 0) {
            return 0;
        }
    }
    return 1;
}

void* generate(void* info)
{
    struct config* conf = (struct config*)info;
    for (long long i = conf->A;; i = (i == conf->B ? conf->A : i + 1)) {
        if (isprime(i)) {
            pthread_mutex_lock(conf->mutex);
            while (*conf->value != -1) {
                pthread_cond_wait(conf->waiter, conf->mutex);
            }
            *conf->value = i;
            pthread_mutex_unlock(conf->mutex);
            pthread_cond_signal(conf->waiter);
        }
    }
}

int main(int argc, char* argv[])
{
    long long A = atoll(argv[1]);
    long long B = atoll(argv[2]);
    int N = atoi(argv[3]);

    long long value = -1;
    pthread_cond_t waiter;
    if (0 != pthread_cond_init(&waiter, NULL)) {
        perror("cond_init");
    }
    pthread_mutex_t mutex;
    if (0 != pthread_mutex_init(&mutex, NULL)) {
        perror("cond_init");
    }

    struct config gen_config = {
        .A = A, .B = B, .waiter = &waiter, .mutex = &mutex, .value = &value};
    pthread_t generator;
    if (0 != pthread_create(&generator, NULL, &generate, &gen_config)) {
        perror("pthread_create");
    }

    while (N--) {
        pthread_mutex_lock(&mutex);
        while (value == -1) {
            pthread_cond_wait(&waiter, &mutex);
        }
        printf("%lld\n", value);
        value = -1;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&waiter);
    }
    pthread_cancel(generator);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&waiter);
}
