#include <bits/stdc++.h>

using namespace std;

class Tree{
	vector<vector<tuple<int,int,int>>> t;
	void build(int v, int tl, int tr, vector<int>& f){
		if(tl == tr){
			t[v] = {make_tuple(f[tl],-1,-1)};
			return;
		}
		int tm = (tl + tr) / 2;
		int vL = v * 2;
		int vR = v * 2 + 1;
		build(vL, tl, tm, f);
		build(vR, tm + 1, tr, f);
		int ptrL = 0;
		for(int ptrR = 0; ptrR < t[vR].size(); ptrR++){
			while(ptrL < t[vL].size() && get<0>(t[vL][ptrL]) < get<0>(t[vR][ptrR])){
				t[v].push_back(make_tuple(get<0>(t[vL][ptrL]), ptrL, ptrR - 1));
				ptrL++;
			}
			t[v].push_back(make_tuple(get<0>(t[vR][ptrR]), ptrL - 1, ptrR)); // ==?
		}
		for(; ptrL < t[vL].size(); ptrL++){
			t[v].push_back(make_tuple(get<0>(t[vL][ptrL]), ptrL, t[vR].size() - 1));
		} 
	}
public:
	Tree(vector<int>& v){
		int n = v.size();
		t.resize(4 * n + 10, vector<tuple<int,int,int>>());
		build(1, 0, n - 1, v);
	}
	int query(int v, int tl, int tr, int l, int r, int topBound, int id = -100){ //last less_or_equal
		if(l > r)
			return 0;
		if(id == -100){
			id = upper_bound(t[v].begin(), t[v].end(), make_tuple(topBound, 1e9, 1e9)) - t[v].begin();
			id--;
		}
		if((tl == l && tr == r) || id == -1){
			return id + 1;
		}	
		int tm = (tl + tr) / 2;
		return query(v * 2, tl, tm, l, min(r, tm), topBound, get<1>(t[v][id])) +
		query(v * 2 + 1, tm + 1, tr, max(l, tm + 1), r, topBound, get<2>(t[v][id]));
	}
};

int main(){
	int n = 0;
	cin >> n;
	vector<int> v(n);
	for(auto &t:v){
		cin >> t;
	}
	Tree tree = Tree(v);
	while(true){
		int l, r, L, R;
		cin >> l >> r >> L >> R;
		cout << tree.query(1, 0, n - 1, l - 1, r - 1, R)
		- tree.query(1, 0, n - 1, l - 1, r - 1, L - 1) << endl;
	}
}
