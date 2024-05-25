#include "xxx.hpp"
#include <vector>

class Solution {
public:
  vector<int> multiply(const vector<int> &nums) {
    auto sz = nums.size();
    vector<int> right(sz, 1);
    for (int i = sz - 2; i >= 0; i--) {
      right[i] = right[i + 1] * nums[i + 1];
    }
    long long m = 1;
    vector<int> ret;
    for (int i = 0; i < sz; i++) {
      int val = m * right[i];
      // for next round
      m *= nums[i];
      ret.push_back(val);
    }
    return ret;
  }
};
