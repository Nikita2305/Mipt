#include <iostream>
#include <vector>
#include <time.h>

/*
Теорема - Матожидание времени работы есть _O_(nlogn)
Док-во:
	L1: 1/1 + 1/2 + 1/3 + 1/4 + ... + 1/n = O(logn)
	
	Обозначим через D = массив A после сортировки. 
	D1 <= D2 <= .. <= Dn
	
	С какой вероятностью Di и Dj когда либо сравнятся? Где i < j
	Утверждается, что с матожиданием E(i,j) = 2/(j-i+1);
	Это исходит и з рассуждений что все когда-то станут пивотом.
	И если им станет i/j - победа
	Но если раньше этого им станет Dk, где i < k < j, то это разделит наши числа и они не сравнятся.
	Отсюда 2 / (j - i + 1) 

	Тогда E(сравнений) = Сумма по всем возможным i,j: E(i,j) = 
	= Сумма по k = 2..n : 2(n-k+1)/k <= 2*n*(Сумма по всем k = 2..n : 1/k) = O(n*log(n))
*/

int pivotIndex(std::vector<int>& v, int l, int r){
	int rnd = l + rand() % (r - l + 1);
	std::swap(v[rnd],v[r]);
	int ptrL = l;
	for (int i = l; i < r; ++i) {
		if (v[i] <= v[r]) {
			std::swap(v[i], v[ptrL]);
			ptrL++;
		}
	}
	std::swap(v[ptrL], v[r]);
	return ptrL;
}

void sort(std::vector<int>& v, int l, int r) {
	if(l >= r)
		return;	
	int pivot = pivotIndex(v, l, r);
	sort(v, l, pivot);
	sort(v, pivot + 1, r);
}

main(){
	std::ios::sync_with_stdio(false);
	std::cin.tie(0);	
	srand(time(0));
	int n = 0;
	std::cin >> n;
	std::vector<int> v(n);
	for (auto &t:v) {
		std::cin >> t;
	}
	sort(v, 0, v.size() - 1);
	for (auto t : v) {
		std::cout << t << " ";
	}
}
