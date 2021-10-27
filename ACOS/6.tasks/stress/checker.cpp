#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>

using namespace std;

const double EPS = 1e-2;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    return -1;
  }

  ifstream solve1(argv[1]);
  ifstream solve2(argv[2]);

  vector<double> v, r;

  double x;
  while (solve1 >> x) {
    v.push_back(x);
  }
  
  while (solve2 >> x) {
    r.push_back(x);
  }

  if (v.size() != r.size()) {
    cout << "FAIL";
    return 0;
  }

  size_t n = v.size();

  for (size_t i = 0; i < n; ++i) {
    if (abs(v[i] - r[i]) > EPS) {
      cout << "FAIL";
      return 0;
    }
  }

  cout << "OK";
}
