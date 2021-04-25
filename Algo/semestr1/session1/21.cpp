#include <iostream>
#include <vector>

/*
LSD - Least Significant Digit
MSD - Most Significant Digit

Альтернативная реализация(чтобы не прибавлять/вычитать единичку) в 19/25 строках соотв:
В 25ой строке идём сверху вниз и используем необходимый байт, а не значение на единицу меньше его
И вычитаем, собственно из него 1
*/

void LSDsort(std::vector<int>& v){
	int digits = 32;
	int blockSize = 8;
	int mask = (1 << blockSize) - 1;
	for(int blockId = 0; blockId < digits/blockSize; ++blockId){
		std::vector<int> count(mask + 2);
		std::vector<int> q(v.size());
		for(int i = 0; i < v.size(); ++i){
			count[((v[i] >> blockId * blockSize) & mask) + 1]++;
		}
		for(int i = 1; i < count.size(); i++){
			count[i] += count[i - 1];
		}
		for(int i = 0; i < v.size(); ++i){
			q[count[(v[i] >> blockId * blockSize) & mask]] = v[i];
			count[(v[i] >> blockId * blockSize) & mask]++;
		}
		v = q;
	}
}

int main(){
	std::vector<int> v = {1,2,1,3,2,3,4,10005, 101083, 18383};
	LSDsort(v);
	for(auto t:v)
		std::cout << t << " ";
}
