#include <functional>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

int main() {
  int t = 0;
  cin >> t;
  while (t--) {
    int serial;
    int n;
    cin >> serial >> n;
    vector<int> nums(n, 0);
    for (int i = 0; i < n; i++) {
      cin >> nums[i];
    }

    priority_queue<int> max_pq;
    priority_queue<int, vector<int>, greater<int>> min_pq;

    cout << serial << " " << (n + 1) / 2 << endl;
    for (int i = 0, sz = 0; i < n; i++) {
      sz += 1;
      max_pq.push(nums[i]);

      while (max_pq.size() > sz / 2) {
        auto cur = max_pq.top();
        max_pq.pop();
        min_pq.push(cur);
      }
      // so max_pq can have one more
      while (min_pq.size() > sz / 2) {
        auto cur = min_pq.top();
        min_pq.pop();
        max_pq.push(cur);
      }
      if (sz & 1) {
        cout << max_pq.top() << " ";
      }
      if (sz % 20 == 0) {
        cout << endl;
      }
    }
    cout << endl;
  }
}
