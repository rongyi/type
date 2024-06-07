#include <algorithm>
#include <cmath>
#include <ios>
#include <iostream>
#include <limits>
#include <vector>
using namespace std;

int main() {
  std::ios::sync_with_stdio(false);
  int n, d;
  cin >> n >> d;
  using pdd = pair<double, double>;
  vector<pdd> ranges(n);
  for (int i = 0; i < n; i++) {
    int x, y;
    cin >> x >> y;

    if (y > d) {
      cout << -1 << endl;
      return 0;
    }
    auto len = sqrt(d * d - y * y);
    ranges[i].first = x - len;
    ranges[i].second = x + len;
  }
  sort(ranges.begin(), ranges.end(), [](pdd &l, pdd &r) {
    if (l.second == r.second) {
      return l.first < r.first;
    }
    return l.second < r.second;
  });
  // sort(ranges.begin(), ranges.end());

  int ret = 0;
  // double limist still a postive number, wtf
  double prev_radar = numeric_limits<int>::min();

  for (int i = 0; i < n; i++) {
    if (ranges[i].first > prev_radar) {
      ret += 1;
      prev_radar = ranges[i].second;
    }
  }

  cout << ret << endl;
  return 0;
}
