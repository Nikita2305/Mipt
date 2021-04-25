#include <iostream>
#include <vector>

/*
T(n) = 1/n * (sum(k=1..n, max(T(k), T(n - k)))) + c * n;
Покажем T(n) <= p * n;
Пусть T(1) = 1; База - очев.
Тогда T(n) = 2/n (T(n-1) + .. + T(n/2)) + c * n <= 2/n * p (n-1 + ... + n/2) + c * n
<= 2/n * p * (1 + .. + n-1 - 1 - .. - (n/2 - 1)) + c * n = 2/n * p * (n(n-1)/2 - n/2(n/2 - 1)/2) + c * n = 
= p * (n-1 - (n/2 - 1)/2) + c*n = p / 4 (4n - 4 - n + 2) + c = p * (3n - 2) / 4 + c * n <= n (3/4p + c) <= p*n
Значит c <= 1/4p, p >= 4c
*/

int pivotInd(std::vector<int>& v, int l, int r) {
	std::swap(v[r], v[l + rand() % (r - l + 1)]);
	int ptrL = l;
	for (int i = l; i < r; ++i) {
		if (v[i] < v[r]) {
			std::swap(v[ptrL], v[i]);
			ptrL++;
		}
	}
	std::swap(v[ptrL], v[r]);
	return ptrL;
}

int kth(std::vector<int>& v, int l, int r, int k) {
	int pivot = pivotInd(v, l, r);
	if (pivot - l + 1 == k) {
		return v[pivot];
	} else if (pivot - l + 1 < k) {
		return kth(v, pivot + 1, r, k - (pivot - l + 1));
	} else {
		return kth(v, l, pivot - 1, k);
	}
}

int main(){
	int n = 0, k = 0;
	std::cin >> n >> k;
	std::vector<int> v(n);
	for (auto &t:v) {
		std::cin >> t;
	}
	std::cout << kth(v, 0, n - 1, k);
}
