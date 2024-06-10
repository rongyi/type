#include <iostream>
#include <vector>

using namespace std;

int a[4][4];

bool check(int x, int y) {
  int sum = 0;
  for (int j = 0; j < 4; j++) {
    if (a[x][j]) {
      sum += 1;
    }
  }
  for (int i = 0; i < 4; i++) {
    if (a[i][y]) {
      sum += 1;
    }
  }
  if (a[x][y]) {
    sum -= 1;
  }
  return sum & 1;
}
int main() {
  ios::sync_with_stdio(false);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      char c;
      cin >> c;
      if (c == '+') {
        a[i][j] = 1;
      } else {
        a[i][j] = 0;
      }
    }
  }

  vector<pair<int, int>> res;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (check(i, j)) {
        res.push_back({i + 1, j + 1});
      }
    }
  }
  cout << res.size() << endl;
  for (auto &p : res) {
    cout << p.first << " " << p.second << endl;
  }
  return 0;
}
