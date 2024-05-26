#include "xxx.hpp"
#include <functional>
#include <queue>
#include <vector>

class Solution {
public:
  void insert(int num) {
    sz += 1;
    max_pq.push(num);
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
  }

  double getMedian() {
    // maxpq can have one more than minpq
    // so middle is in maxpq
    // else we get from two pq
    if (sz & 1) {
      auto cur = max_pq.top();
      return cur;
    } else {
      auto val1 = max_pq.top();
      auto val2 = min_pq.top();
      return (val1 + val2) / 2.0;
    }
  }

private:
  int sz = 0;
  priority_queue<int, vector<int>, greater<int>> min_pq;
  priority_queue<int, vector<int>, less<int>> max_pq;
};

int main() {
  Solution so;
  vector<int> input{-1, -1, 2};
  for (auto &num : input) {
    so.insert(num);
    cout << so.getMedian() << endl;
  }
}
