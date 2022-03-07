#include <assert.h>
#include <immintrin.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void swap_strings(double* mtx, int str1, int str2, int len, int strsize)
{
    int i;
    for (i = 0; i + 4 <= len; i += 4) {
        __m256d a = _mm256_load_pd(mtx + strsize * str1 + i);
        __m256d b = _mm256_load_pd(mtx + strsize * str2 + i);
        _mm256_store_pd(mtx + strsize * str2 + i, a);
        _mm256_store_pd(mtx + strsize * str1 + i, b);
    }
    for (; i < len; i++) {
        double temp = mtx[strsize * str1 + i];
        mtx[strsize * str1 + i] = mtx[strsize * str2 + i];
        mtx[strsize * str2 + i] = temp;
    }
}

void devide_string(double* mtx, int str, int len, double coef, int strsize)
{
    _Alignas(32) double coef_arr[4] = {coef, coef, coef, coef};
    __m256d coefpd = _mm256_load_pd(coef_arr);
    int i;
    for (i = 0; i + 4 <= len; i += 4) {
        __m256d a = _mm256_load_pd(mtx + strsize * str + i);
        a = _mm256_div_pd(a, coefpd);
        _mm256_store_pd(mtx + strsize * str + i, a);
    }
    for (; i < len; i++) {
        mtx[strsize * str + i] /= coef;
    }
}

void subtract_strings(
    double* mtx,
    int from,
    int what,
    int len,
    double coef,
    int strsize)
{
    ////printf("Substracting %d from %d with coef: %lf\n", what, from, coef);
    _Alignas(32) double coef_arr[4] = {coef, coef, coef, coef};
    __m256d coefpd = _mm256_load_pd(coef_arr);
    int i;
    for (i = 0; i + 4 <= len; i += 4) {
        __m256d a = _mm256_load_pd(mtx + strsize * from + i);
        __m256d b = _mm256_load_pd(mtx + strsize * what + i);
        b = _mm256_mul_pd(b, coefpd);
        a = _mm256_sub_pd(a, b);
        _mm256_store_pd(mtx + strsize * from + i, a);
    }
    for (; i < len; i++) {
        mtx[strsize * from + i] -= mtx[strsize * what + i] * coef;
    }
}

int main()
{
    const double eps = 1e-6;
    int n = 0;
    scanf("%d", &n);
    int len = (n + 1) + 3 & (-4);
    double* temp = malloc(sizeof(double*) * n * len + 32);
    double* mtx = (double*)(((long long)((void*)temp + 31)) & (-32));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n + 1; j++) {
            scanf("%lf", &mtx[len * i + j]);
        }
    }
    for (int str = 0; str < n; str++) {
        int found = -1;
        for (int str1 = str; str1 < n; str1++) {
            if (fabs(mtx[len * str1 + str]) > eps) {
                found = str1;
                break;
            }
        }
        assert(found != -1);
        swap_strings(mtx, str, found, n + 1, len);
        devide_string(mtx, str, n + 1, mtx[len * str + str], len);
        for (int str1 = 0; str1 < n; str1++) {
            if (str1 != str) {
                subtract_strings(
                    mtx, str1, str, n + 1, mtx[len * str1 + str], len);
            }
        }
    }
    for (int i = 0; i < n; i++) {
        printf("%.10f\n", mtx[len * i + n]);
    }
    free(temp);
}
