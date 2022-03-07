#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <assert.h>

extern double avx_sin(double x);

typedef unsigned long long ull;

const double EPS = 1e-6;
void test(double x) {
    printf("%.19f\n",x);
    printf("%.19f\n",avx_sin(x));
    printf("%.19f\n",fabs(sin(x) - avx_sin(x)));
    assert(fabs(sin(x) - avx_sin(x)) < EPS);
}

int main() {

 
  for (int i = 0; i < 100; ++i) {
    double x = ((double)(rand() % 10000)) / 10000.0;
    test(x);
  } 

  for (int i = 0; i < 100; ++i) {
    double x = rand() % 100 + ((double)(rand() % 10000)) / 10000.0;
    test(x);
  } 

  for (int i = 0; i < 100; ++i) {
    double x = 10000 + rand() % 10000 + ((double)(rand() % 10000)) / 10000.0;
    test(x);
  } 
    for (int i = 0; i < 100; ++i) {
    double x = -10000 + rand() % 10000 + ((double)(rand() % 10000)) / 10000.0;
    test(x);
  } 
}
