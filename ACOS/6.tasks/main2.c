#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include <math.h>

extern float dot_product(size_t N, const float *A, const float *B);

int main() {
  for (int i = 0; i < 10000; ++i) {
    const int N = 100;
    int n = 1 + rand() % N;
    float* a = malloc(sizeof(float) * n);
    float* b = malloc(sizeof(float) * n);
    float ret = 0.f;
    for (int i = 0; i < n; i++) {
        a[i] = rand() % 50 + (float)(rand() % 1000) / 1000;
        b[i] = rand() % 50 + (float)(rand() % 1000) / 1000;
        printf("%.19f %.19f\n", a[i], b[i]);
        ret += 1.f * a[i] * b[i];
    }
    float rmy = dot_product(n, a, b);
    printf("%d\n", n);
    printf("%.19f\n", ret);
    printf("%.19f\n", rmy);
    printf("%.19f\n\n", fabs(ret - rmy));
    assert(fabs(ret - rmy) < 0.0001f);

    free(a);
    free(b);
  }
}
