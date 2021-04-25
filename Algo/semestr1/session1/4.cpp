#include <iostream>

struct Item {
	Item(int value): value(value), previous(nullptr) {}
	int value;
	Item* previous;
};

struct Stack {
	Item* topItem = nullptr;
	int size = 0;
	void push(int x){
		size++;
		Item* temp = topItem;
		topItem = new Item(x);
		topItem->previous = temp;
	}
	void pop(){
		size--;
		Item* temp = topItem->previous;
		delete topItem;
		topItem = temp;
	}
	int& top(){
		return topItem->value;
	}
};

int main(){
	Stack st = Stack();
	st.push(5);
	std::cout << st.top();
	st.pop();
	std::cout << st.size;
}

