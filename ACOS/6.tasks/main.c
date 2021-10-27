#include <stdio.h>
#include <stdlib.h>

double very_important_function(int N, const float* A, const float* B, float* C);

int main() {
    const int N = 16;
    float* A = malloc(sizeof(int) * N);
    float* B = malloc(sizeof(int) * N);
    float* C = malloc(sizeof(int) * N);
    for (int i = 0; i < N; i++) {
        A[i] = i;
        B[i] = 1;
    }
    float x = very_important_function(N, A, B, C);
    for (int i = 0; i < N; i++) {
        printf("%f\n", C[i]);
    }
    printf("Ans: %f", x);
}
