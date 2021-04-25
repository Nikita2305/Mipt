#include <bits/stdc++.h>

#define int long long

using namespace std;

class Fenvik2D{
	vector<vector<vector<int>>> s;
	int getSum(int x, int y, int z){
		if(x == -1 || y == -1 || z == -1)
			return 0;
		int sum = 0;
		for(int i = x; i >= 0; i = (i & (i + 1)) - 1){
			for(int j = y; j >= 0; j = (j & (j + 1)) - 1){
				for(int k = z; k >= 0; k = (k & (k + 1)) - 1){
					sum += s[i][j][k];
				}
			}
		}
		return sum;
	}
public:
	Fenvik2D(int n){
		s.resize(n, vector<vector<int>>(n, vector<int>(n)));
	}

	int get(int x1, int y1, int z1, int x2, int y2, int z2){
		return 0ll + getSum(x2,y2,z2) - getSum(x1-1,y2,z2) - getSum(x2,y1-1,z2) - getSum(x2,y2,z1-1) + getSum(x1-1,y1-1,z2) + getSum(x1-1,y2,z1-1) + getSum(x2,y1-1,z1-1) - getSum(x1-1,y1-1,z1-1);
	}

	void upd(int x, int y, int z, int dt){
		for(int i = x; i < s.size(); i = i | (i + 1)) {
			for(int j = y; j < s.size(); j = j | (j + 1)) {
				for(int k = z; k < s.size(); k = k | (k + 1)) {
					s[i][j][k] += dt;
				}
			}
		}
	}
};

main(){
	int n = 0;
	cin >> n;
	Fenvik2D tree(n);
	while(true){
		int m;
		cin >> m;
		if (m == 1){
			int x,y,z,dt;
			cin >> x >> y >> z >> dt;
			tree.upd(x, y, z, dt);
		} else if(m == 2) {
			int x1,y1,z1,x2,y2,z2;
			cin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
			cout << tree.get(x1, y1, z1, x2, y2, z2) << endl;
		} else {
			return 0;
		}
	}
}
