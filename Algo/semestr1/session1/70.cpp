#include <bits/stdc++.h>

using namespace std;

class Fenvik{
	vector<int> sum;
public:
	Fenvik(vector<int>& v){
		vector<int> pref(v.size() + 1);
		for(int i = 1; i <= v.size(); i++){
			pref[i] = pref[i - 1] + v[i - 1];
		}
		sum.resize(v.size());
		for(int i = 0; i < v.size(); i++){
			sum[i] = pref[i + 1] - pref[i & (i + 1)];
		}
	}
	int get(int bound){
		if(bound == -1)
			return 0;
		//cout << "bound:" << bound << endl;
		int ans = 0;
		for(int i = bound; i >= 0; i = ((i & (i + 1)) - 1)){
			//cout << i << " next: " << ((i & (i + 1)) - 1) << endl;
			ans += sum[i];
		}
		//cout << "return: " << ans << endl; 
		return ans;
	}
	void upd(int ind, int dt){
		for(int i = ind; i < sum.size(); i = i | (i + 1)) {
			sum[i] += dt;
		}
	}
};

int main(){
	int n = 0;
	cin >> n;
	vector<int> v(n);
	for(int i = 0; i < n; i++){
		cin >> v[i];
	}
	Fenvik f(v);
	while(true){
		char c;
		int a, b;
		cin >> c >> a >> b;
		if(c == 'u'){
			a--;
			f.upd(a,b);
		} else if(c == 'g'){
			a--;b--;
			cout << f.get(b) - f.get(a - 1) << endl;
		}
	}
}
