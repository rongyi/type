#include <algorithm>
#include <array>
#include <functional>
#include <ios>
#include <iostream>
#include <map>
#include <queue>
#include <strings.h>
#include <vector>
using namespace std;

int main() {
  int n;
  cin >> n;
  vector<array<int, 3>> cows;
  for (int i = 0; i < n; i++) {
    int start, end;
    cin >> start >> end;
    cows.push_back({start, end, i});
  }
  sort(cows.begin(), cows.end());
  vector<int> taken(n, 0);
  int total = 0;
  priority_queue<array<int, 2>, vector<array<int, 2>>, greater<array<int, 2>>>
      pq;
  for (auto &cow : cows) {
    auto start = cow[0];
    auto end = cow[1];
    auto idx = cow[2];
    if (pq.empty() || start <= pq.top()[0]) {
      int sz = pq.size();
      taken[idx] = sz;
      pq.push({end, sz});
      total += 1;
    } else {
      auto cur = pq.top();
      pq.pop();
      cur[0] = end;
      taken[idx] = cur[1];
      pq.push(cur);
    }
  }

  cout << total << endl;
  for (int i = 0; i < n; i++) {
    cout << taken[i] + 1 << endl;
  }
}
