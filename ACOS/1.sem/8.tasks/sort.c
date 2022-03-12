#include <stdio.h>

void merge(long long* v, long long* buf, int l, int m, int r) {
    int ptrl = l;
    int ptrr = m;
    int ptr = l;
    while (ptrl < m || ptrr < r) {
        if (ptrl == m || (ptrl < m && ptrr < r && v[ptrr] < v[ptrl])) {
            buf[ptr] = v[ptrr];
            ptrr++;
            ptr++;
            continue;
        }
        if (ptrr == r || (ptrl < m && ptrr < r && v[ptrr] >= v[ptrl])) {
            buf[ptr] = v[ptrl];
            ptrl++;
            ptr++;
            continue;
        }
    }
    for (int i = l; i < r; i++) {
        v[i] = buf[i];
    }
}

void mergesort(long long* v, long long* buf, int l, int r) {
    if (l >= r - 1) {
        return;
    }
    int m = (l + r) / 2;
    mergesort(v, buf, l, m);
    mergesort(v, buf, m, r);
    merge(v, buf, l, m, r);
}

int main() {
    int n = 0;
    scanf("%d", &n);
    long long arr[n];
    long long buf[n];
    for (int i = 0; i < n; i++) {
        scanf("%lld", arr + i);
    }
    mergesort(arr, buf, 0, n);
    for (int i = 0; i < n; i++) {
        printf("%lld ", arr[i]);
    }
}
