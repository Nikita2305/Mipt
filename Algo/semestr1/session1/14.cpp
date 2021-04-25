#include <iostream>
#include <vector>

int countInversions(std::vector<int>& v, std::vector<int>& buf, int l, int r){
	if (l == r)
		return 0;
	int count = 0;
	int m = (l + r) / 2;
	count += countInversions(v, buf, l, m);
	count += countInversions(v, buf, m + 1, r);
	int ptrL = 0;
	int ptrR = 0;
	
	while (l + ptrL <= m || m + 1 + ptrR <= r) {
		if (l + ptrL > m) {
			buf[l + ptrL + ptrR] = v[m + 1 + ptrR];
			++ptrR;
		} else if (m + 1 + ptrR > r) {
			buf[l + ptrL + ptrR] = v[l + ptrL];
			count += ptrR;
			++ptrL;
		} else {
			if (v[l + ptrL] < v[m + 1 + ptrR]) {
				buf[l + ptrL + ptrR] = v[l + ptrL];
				count += ptrR;
				++ptrL;
			} else {
				buf[l + ptrL + ptrR] = v[m + 1 + ptrR];
				++ptrR;
			}
		}
	}
	for (int i = l; i <= r; ++i) {
		v[i] = buf[i];
	}
	return count;
}

int main(){
	std::vector<int> v = {1,4,3,2,5};
	std::vector<int> buf(v.size());
	std::cout << countInversions(v, buf, 0, v.size() - 1);
}
