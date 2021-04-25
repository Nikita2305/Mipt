#include <bits/stdc++.h>

using namespace std;
/*
Отложенные операции на отрезке
Пусть наш массив А: A0 .. An-1
Нахуярим массив B: B0 .. Bn-1
B0 = A0
B1 = A1 - A0
...
Bn-1 = An-1 - a(n-2)
Тогда чтобы прибавить val на отрезке l,r - это значит прибавить val в точке B_l и отнять val в точке B_(r+1), r<n-1
Заметим Ai = B0 + .. + Bi
Тогда A0+...+Ak = B0 + (B0 + B1) + ... + (B0 + ... + Bk) = (k+1)*B0 + k*B1 + ... + Bk = 
= k * (B0 + ... + Bk) + (B0*(1 - 0) + ... + Bk(1 - k))
Поимеем два Фенвика, один на сумму B, другой на недефолт сумму B(Ci = Bi(1 - i))
Запрос на апдейт точки = апдейт отрезка
Запрос на get - описан выше.

Двумерный случай.
Пусть наш массив: Ai,j
Пусть Bi,j = Ai,j - Ai-1,j - Ai,j-1 + Ai-1,j-1 // 0 в случае выхода за границу
Тогда чтобы прибавить на подпрямоугольнике - заметим, достаточно поменять только Bx,y += val, B(x1+1,y1+1) -= val
Ок, тогда давайте научимся искать сумму на подпрямоугольнике. Аx,y = Сумм(i<=x, j<=y, Bij)
Тогда сумма на префиксе A00 +  ... + Ak,t
(...) 
Тогда делаем 4 фенвика, какие там нужно и побеждаем.
*/

class Fenvik{
	vector<int> s;	
public:
	Fenvik(int n){
		s.resize(n);
	}
	int get(int x){
		int sum = 0;
		for(; x >= 0; x = (x & (x + 1)) - 1){
			sum += s[x];
		}
		return sum;
	}
	void upd(int x, int dt){
		for(; x < s.size(); x = x | (x + 1)){
			s[x] += dt;
		}
	}
};

int sum(int id, Fenvik& B, Fenvik& Bmult){
	return id * B.get(id) + Bmult.get(id);
}

int q(int l, int r, Fenvik& B, Fenvik& Bmult){
	return sum(r, B, Bmult) - sum(l - 1, B, Bmult);
}

void upd(int ind, int val, Fenvik& B, Fenvik& Bmult){
	B.upd(ind, val);
	Bmult.upd(ind, val * (1 - ind));
}

int main(){
	int n = 0;
	cin >> n;
	Fenvik B(n);
	Fenvik Bmult(n);
	while(true){
		char c;
		cin >> c;
		if(c == 'g'){
			int l, r;
			cin >> l >> r;
			l--;r--;
			cout << q(l, r, B, Bmult) << endl;
		}else if(c == 'u'){
			int l, r, dt;
			cin >> l >> r >> dt;
			l--;r--;
			upd(l, dt, B, Bmult);
			if(r != n-1)
				upd(r + 1, -dt, B, Bmult);
		}
	}
}
