#include <algorithm>
#include <array>
#include <functional>
#include <ios>
#include <iostream>
#include <queue>
#include <strings.h>
#include <vector>
using namespace std;

int main() {
  ios::sync_with_stdio(false);
  int n = 0;
  cin >> n;
  // start, end, index
  vector<array<int, 3>> cows(n);
  for (int i = 0; i < n; i++) {
    cin >> cows[i][0] >> cows[i][1];
    cows[i][2] = i;
  }
  sort(cows.begin(), cows.end());

  int total = 0;
  vector<int> taken(n, 0);
  using i2 = array<int, 2>;
  // end time and index
  priority_queue<i2, vector<i2>, greater<i2>> pq;

  for (int i = 0; i < n; i++) {
    auto start = cows[i][0];
    auto end = cows[i][1];
    auto idx = cows[i][2];
    // 每次都去找之前最早结束的那个牛栏，找到就复用，找不到就
    // 新开一个牛栏，编号顺延
    // 什么情况下能用？这个牛栏里的牛结束吃草时间早于下一个牛开始的
    if (pq.empty() || pq.top()[0] >= start) {
      int next_idx = pq.size();
      pq.push({end, next_idx});
      taken[idx] = next_idx;
      total += 1;
    } else {
      // latest finished room, we can take
      // {endtime, root index}
      auto cur = pq.top();
      pq.pop();
      cur[0] = end;
      taken[idx] = cur[1];

      pq.push(cur);
    }
  }

  cout << total << endl;
  for (auto &val : taken) {
    cout << val + 1 << endl;
  }
  return 0;
}
