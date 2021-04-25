#include <iostream>
#include <vector>

int main(){
	std::vector<int> v(10);
	for (int i = 1; i < v.size(); ++i) {
		v[i] = v[i-1] + i % 5;
	}
	for (auto t:v) 
		std::cout << t << " ";
	while(true) {
		int c;
		std::cin >> c;
		int l = 0;
		int r = v.size();
		while (l < r) {
			int m = (l + r) / 2;
			if (v[m] > c)
				r = m;
			else
				l = m + 1;
		}
		std::cout << r << "is first greater index" << std::endl;
	}
}
