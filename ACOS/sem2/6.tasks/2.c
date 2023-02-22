#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>

typedef struct Item {
    struct Item *next;
    int64_t      value;
} item_t;

typedef struct List {
    item_t* fake;
    item_t* tail;  
} list_t;

void init_list(list_t* list) {
    list->fake = malloc(sizeof(item_t));
    list->fake->next = list->fake;
    list->tail = list->fake;
}

void destroy_list(list_t* list) {
    while (list->fake->next != list->fake) {
        item_t* first = list->fake->next;
        list->fake->next = first->next;
        free(first);
    }
    free(list->fake);
}

void print_list(list_t* list) {
    item_t* item = list->fake->next;
    while (item != list->fake) {
        printf("%ld ", item->value);
        item = item->next;
    }
    printf("\n");
}

void push_back(list_t* list, int64_t value) {
    item_t* item = malloc(sizeof(item_t));
    item->value = value;
    item->next = list->fake;
    list->tail->next = item;
    list->tail = item;
}

struct config {
    int64_t lower;
    int64_t upper;
    _Atomic int* is_ready;
    list_t* list;
};

void* producer(void* info) {
    struct config* config = (struct config*)info;
    for (int64_t i = config->lower; i < config->upper; i++) {
        while (1) { 
            if (atomic_exchange(config->is_ready, 0)) {
                break;
            }
            sched_yield();
        }
        push_back(config->list, i);
        atomic_store(config->is_ready, 1);
    }
}

int main(int argc, char* argv[]) {
    int N = atoi(argv[1]);
    int k = atoi(argv[2]);

    list_t list;
    init_list(&list);
    _Atomic int is_ready = 1;
    struct config configs[N];
    pthread_t threads[N];
    for (int i = 0; i < N; i++) {
        configs[i].lower = k*i;
        configs[i].upper = k*(i+1);
        configs[i].is_ready = &is_ready;
        configs[i].list = &list;
    }
    for (int i = 0; i < N; i++) {
        if (0 != pthread_create(&threads[i], NULL, &producer, &configs[i])) {
            perror("pthread_create");
        }
    }
    for (int i = 0; i < N; i++) {
        if (0 != pthread_join(threads[i], NULL)) {
            perror("pthread_join");
        }
    }
    print_list(&list);
    destroy_list(&list);
}
