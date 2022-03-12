#include <stdio.h>

long long A = 2, B = 3, C = 4, D = 5;

extern long long calculate();
extern long long R;

int main() {
    calculate();
    printf("%lld", R);
}
