#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern void sum(int N, const int *A, const int* B, int* R);

int main() { 
    srand(time(0));
    int* A = malloc(100 * sizeof(int));
    int* B = malloc(100 * sizeof(int));
    int* C = malloc(100 * sizeof(int));

    for (int i = 0; i < 100; i++) {
        A[i] = i;
        B[i] = i + 1;
    }
   
    sum(100, A, B, C);
 
    for (int i = 0; i < 100; i++) {
        printf("%d ", C[i]);
    }
}
