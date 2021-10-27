#include <iostream>
#include <vector>
#include <set>
#include <random>
#include <fstream>
#include <chrono>

using namespace std;

mt19937 rnd(chrono::steady_clock::now().time_since_epoch().count());

bool panic = false;

void MultiplyLine(vector<long long>& v, long long x) {
  for (auto& i: v) {
    i *= x;
    if (i > 1e9) panic = true;
  }
} 

void AdditionLine(vector<long long>& v, vector<long long>& t) {
  for (size_t i = 0; i < v.size(); ++i) {
    v[i] += t[i];
    if (v[i] > 1e9) panic = true;
  }
}

// Запускать ./a.out размер матрицы, название файла для теста, название файла
// для ответа

int main(int argc, char* argv[]) {
  if (argc != 4) {
    return -1;
  }
  long long n = atoi(argv[1]);
  ofstream test(argv[2]);
  ofstream solve(argv[3]);

  set<long long> s;
  vector<vector<long long>> v(n, vector<long long>(n + 1));
  vector<long long> result(n);

  for (long long i = 0; i < n; ++i) {
    v[i][i] = 1;

    long long x;
    do {
      x = rnd() % 11 - 5;
    } while (s.count(x));
    s.insert(x);
    v[i][n] = x;
  }

  for (int i = 0; i < n; ++i) {
    result[i] = v[i][n];
  }

  for (int i = 0; i < 2000; ++i) {
    if (rnd() % 5) {
      int i = rnd() % n, j = rnd() % n;
      if (i == j) continue;
      AdditionLine(v[i], v[j]);
    } else {
      MultiplyLine(v[rnd() % n], -1);
    }

    if (panic) break;
  }

  test << n << "\n";
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n + 1; ++j) {
      test << v[i][j] << (j == n ? "" : " ");
    }
    test << "\n";
  }

  for (int i = 0; i < n; ++i) {
    solve << result[i] << "\n";
  }
}
