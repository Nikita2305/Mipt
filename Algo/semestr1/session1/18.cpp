#include <iostream>
#include <vector>
#include <algorithm>
/*
Поиск k-й статистики.
T(n) = T(n/5) + T(7n/10) + c*n
T(1) = 1, [a >= 1]
по индукции T(n) <= a * n/5 + a * 7n/10 + c*n <= a*n
Верно при 9a/10 + c <= a, a/10 >= c, [a >= 10c]
*/

int kth(std::vector<int>&, int, int, int);

int pivotInd(std::vector<int>& v, int l, int r) {
	std::vector<int> med;
	med.reserve((r-l+1)/5 + 10);
	std::vector<int> temp;
	for(int i = l; i <= r; i++){
		temp.push_back(v[i]);
		if(i == r || (i - l) % 5 == 4){
			std::sort(temp.begin(), temp.end());
			med.push_back(temp[temp.size()/2]);	
			temp.clear();
		}
	}
	//std::cout << "resolve medians" << std::endl;
	int value = med[0];
	if(med.size() > 1) 
		kth(med, 0, med.size() - 1, med.size() / 2 + 1);
	//std::cout << "end of medians" << std::endl;
	int ind = -1;
	for(int i = l; i <= r; ++i) {
		if(v[i] == value)
			ind = i;
	}
	std::swap(v[ind], v[r]);
	int ptrL = l;
	for(int i = l; i < r; ++i){ 
		if(v[i] < v[r]){
			std::swap(v[i], v[ptrL]);
			ptrL++;
		}
	}
	std::swap(v[r], v[ptrL]);
	return ptrL;
}

int kth(std::vector<int>& v, int l, int r, int k){
	//if(l != r) std::cout << l << " " << r << std::endl;
	int pivot = pivotInd(v, l, r);
	if(k == pivot - l + 1)
		return v[pivot];
	else if(k < pivot - l + 1)
		return kth(v, l, pivot - 1, k);
	else 
		return kth(v, pivot + 1, r, k - (pivot - l + 1));
	
}

int main(){
	std::vector<int> v = {1,3,4,2,5,5,3,10,7,8,10,11,4};
	for(int i = 1; i <= v.size(); ++i){
		std::cout << kth(v, 0, v.size() - 1, i) << std::endl;
	}
}
