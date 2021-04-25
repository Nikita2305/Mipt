#include <bits/stdc++.h>

using namespace std;

class Tree{
	int tl;
	int tr;
	int tm;
	int sum;
	Tree* l;
	Tree* r;
public:
	Tree(int tl, int tr, int sum = 0): tl(tl), tr(tr), tm((tl + tr)/2), sum(sum), l(nullptr), r(nullptr){
	}
	
	int get(int L, int R) {
		if(L > R)
			return 0;
		if(L == tl && R == tr){
			return sum;
		}
		if(!l) l = new Tree(tl, tm);
		if(!r) r = new Tree(tm + 1, tr);
		return l->get(L, min(tm, R)) + r->get(max(tm + 1, L), R);
	}

	Tree* upd(int ind, int val) {
		if(tl == tr){
			return new Tree(tl, tr, val);
		}
		if(!l) l = new Tree(tl, tm);
		if(!r) r = new Tree(tm + 1, tr);

		Tree* ret = new Tree(tl, tr, sum);
		if (ind <= tm) {
			ret->r = r;
			ret->l = l->upd(ind, val);
		} else {
			ret->l = l;
			ret->r = r->upd(ind, val);
		}
		ret->sum = ret->l->sum + ret->r->sum;
		return ret;
	}
};

int main(){
	int n = 0;
	cin >> n;	
	vector<pair<int,int>> values(n);
	for(int i = 0; i < n; i++){
		int c;
		cin >> c;
		values[i] = {c, i};
	}
	sort(values.begin(), values.end());	
	cout << "kek" << endl;
	vector<Tree*> trees(n + 1, nullptr);
	trees[0] = new Tree(0, n - 1);	
	for(int i = 0; i < n; i++){
		cout << i << endl;
		trees[i + 1] = trees[i]->upd(values[i].second, 1);
	}

	int q = 0;
	cin >> q;	
	for(int i = 0; i < q; i++) {
		int l, r, L, R;
		cin >> l >> r >> L >> R;
		l--;r--;
		int count = 0;
		int id = upper_bound(values.begin(), values.end(), make_pair(L, -1)) - values.begin();
		count -= trees[id]->get(l, r);
		id = upper_bound(values.begin(), values.end(), make_pair(R, n + 10)) - values.begin();
		count += trees[id]->get(l, r);
		cout << count << endl;
	}
}
