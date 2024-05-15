#include "xxx.hpp"
#include <deque>

class Solution {
public:
  int lastRemaining(int n, int m) {
    deque<int> q;
    for (int i = 0; i < n; i++) {
      q.push_back(i);
    }
    int erase = 0;
    while (q.size() >= 2) {
      auto cur = q.front();
      q.pop_front();

      erase += 1;
      if (erase == m) {
        erase = 0;
      } else {
        q.push_back(cur);
      }
    }

    return q.front();
  }
};

int main() {
  Solution so;
  auto val = so.lastRemaining(5, 3);
  cout << val << endl;
}
