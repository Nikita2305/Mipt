#include <bits/stdc++.h>

using namespace std;

const int maxn = 1e5;
int t[4*maxn];
int push[4*maxn];
vector<int> a;

void psh(int v, int tl, int tm, int tr) {
	push[v * 2] += push[v];
	push[v * 2 + 1] += push[v];
	t[v * 2] += push[v] * (tm - tl + 1);
	t[v * 2 + 1] += push[v] * (tr - tl);
	push[v] = 0;
}

void build(int v, int tl, int tr){
	if (tl == tr) {
		t[v] = a[tl];
		return;
	}
	int tm = (tl + tr) / 2;
	build(v * 2, tl, tm);
	build(v * 2 + 1, tm + 1, tr);
	t[v] = t[v * 2] + t[v * 2 + 1];
}

int get(int v, int tl, int tr, int l, int r) {
	if (l > r)
		return 0;
	if (tl == l && tr == r) 
		return t[v];
	int tm = (tl + tr) / 2;
	psh(v, tl, tm, tr);
	return get(v * 2, tl, tm, l, min(tm, r)) + get(v * 2 + 1, tm + 1, tr, max(tm + 1, l), r);
}

void upd(int v, int tl, int tr, int l, int r, int dt) {
	if(l > r)
		return;
	if(tl == l && tr == r){
		push[v] += dt;
		t[v] += (tr - tl + 1) * dt;
		return;
	}
	int tm = (tl + tr) / 2;
	psh(v, tl, tm, tr);
	upd(v * 2, tl, tm, l, min(tm, r), dt);
	upd(v * 2 + 1, tm + 1, tr, max(tm + 1, l), r, dt);
	t[v] = t[v * 2] + t[v * 2 + 1];
}

int main(){
	int n = 0;
	cin >> n;
	a.resize(n);
	for(auto &t:a)
		cin >> t;
	int q = 1000;
	build(1, 0, n - 1);
	for(int i = 0; i < q; i++){
		char c;
		int l,r,x;
		cin >> c >> l >> r >> x;
		l--;r--;
		if (c == 'u') {
			upd(1, 0, n - 1, l, r, x);
		} else if(c == 'g') {
			cout << get(1, 0, n - 1, l, r) << endl;
		}
	}
}
