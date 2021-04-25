#include <iostream>
#include <vector>

void sort(std::vector<int>& v){
	std::vector<int> count(100);
	for(auto t:v)
		count[t]++;
	for(int i = 1; i < count.size(); ++i){
		count[i]+=count[i-1];
	}
	std::vector<int> q(v.size());
	for(int i = 0; i < v.size(); ++i) {
		q[count[v[i]-1]] = v[i];
		count[v[i]-1]++;
	}
	v = q;
}

int main(){
	std::vector<int> v = {1,2,1,3,4,4,4};
	sort(v);
	for(auto t:v)
		std::cout << t << " ";
}
