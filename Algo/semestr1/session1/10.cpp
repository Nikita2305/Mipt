#include <iostream>
#include <stack>
#include <string>

// Корректность польской записи - значение num - zn > 0 on each pref, and num - zn = 1 on string

int main(){
	std::stack<int> st;
	std::string token = "";
	while(std::cin >> token){	
		if(token == "stop") 
			break;
		if(token == "+" || token == "*" || token == "-" || token == "/"){
			int a = st.top();
			st.pop();
			int b = st.top();
			st.pop();
			if(token=="+")
				st.push(a + b);
			if(token=="*")
				st.push(a * b);
			if(token=="-")
				st.push(a - b);
			if(token=="/")
				st.push(a / b);
		}else{
			st.push(std::stoi(token));
		}
	}
	std::cout << st.top();
}
