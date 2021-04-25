#include <iostream>
#include <vector>

int main(){
	int n;
	std::cin >> n;
	std::vector<int> v(n);
	for(auto &t:v)
		std::cin >> t;
	std::vector<int> ans(n);
	std::vector<int> st;
	for(int i = 0; i < n; i++){
		while(!st.empty() && v[st.back()] < v[i]) {
			ans[st.back()] = i;
			st.pop_back();
		}
		st.push_back(i);
	}
	for (int i = 0; i < st.size(); ++i) {
		ans[st[i]] = v.size();
	}
	for(int i = 0; i < n; i++)
		std::cout << ans[i] << " ";
}
