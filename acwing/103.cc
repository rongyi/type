#include <iostream>
#include <set>
#include <vector>
using namespace std;
int main() {
  // total cows
  int n;
  // this index cow have height h
  int p;
  // height
  int h;
  // total pairs count
  int m;

  cin >> n >> p >> h >> m;
  // 1..=n put number
  // +1 -1 range
  vector<int> diff(n + 1, 0);

  set<pair<int, int>> uniq;
  for (int i = 0; i < m; i++) {
    int l;
    int r;
    cin >> l >> r;
    uniq.insert({min(l, r), max(l, r)});
  }

  for (auto &p : uniq) {
    auto l = p.first;
    auto r = p.second;
    // range from l + 1 should decrese 1
    diff[l + 1] -= 1;
    // range from r + 1
    diff[r] += 1;
  }

  for (int i = 1; i < diff.size(); i++) {
    diff[i] += diff[i - 1];
  }
  for (int i = 1; i < diff.size(); i++) {
    diff[i] += h;
    cout << diff[i] << endl;
  }
}
