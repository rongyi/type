#include <algorithm>
#include <ios>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

int c;
int l;

int main() {
  ios::sync_with_stdio(false);
  int cow;
  int spf;
  cin >> cow >> spf;

  vector<pair<int, int>> cows;
  for (int i = 0; i < cow; i++) {
    int val1;
    int val2;
    cin >> val1;
    cin >> val2;
    cows.push_back({val1, val2});
  }
  // 按照防晒区间的end来排序，把大的留在后面给其他牛使用
  sort(cows.begin(), cows.end(), [](pair<int, int> &l, pair<int, int> &r) {
    return l.second < r.second;
  });

  map<int, int> spfs;
  for (int i = 0; i < spf; i++) {
    int k;
    int v;
    cin >> k >> v;
    spfs[k] += v;
  }
  int ret = 0;

  // we take small, and leave bigger for bigger range
  for (auto &p : cows) {
    for (int j = p.first; j <= p.second; j++) {
      if (spfs.count(j)) {
        ret += 1;
        spfs[j] -= 1;
        if (spfs[j] == 0) {
          spfs.erase(j);
        }
        break;
      }
    }
  }
  cout << ret << endl;

  return 0;
}
