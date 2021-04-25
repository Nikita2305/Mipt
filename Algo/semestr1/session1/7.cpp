#include <iostream>
#include <stack>

int main(){
	const int INF = 1e9;
	int q;
	std::cin >> q;
	std::stack<int> head;
	int maxOnHead = -INF;
	std::stack<std::pair<int,int>> tail;
	while (q--) {
		char c;
		std::cin >> c;
		if (c == '?') {
			std::cout << std::max(maxOnHead, tail.size() ? tail.top().second : -INF);
		} else if (c == 'a') {
			int x;
			std::cin >> x;
			head.push(x);
			maxOnHead = std::max(maxOnHead, x);
		} else if (c == 'd') {
			if (tail.size() == 0) {
				maxOnHead = -INF;
				while(!head.empty()) {
					tail.push({head.top(), std::max(tail.size() ? tail.top().second : -INF, head.top())});
					head.pop();
				}	
			}
			tail.pop();
		}
	}
}
