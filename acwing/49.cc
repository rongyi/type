#include "xxx.hpp"
#include <functional>
#include <queue>
#include <vector>

class Solution {
public:
  vector<int> getLeastNumbers_Solution(vector<int> input, int k) {
    priority_queue<int, vector<int>, greater<int>> pq;
    for (auto &num : input) {
      pq.push(num);
    }
    vector<int> res;
    while (!pq.empty() && k--) {
      auto cur = pq.top();
      pq.pop();
      res.push_back(cur);
    }
    return res;
  }
};
