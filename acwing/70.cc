#include "xxx.hpp"

class Solution {
public:
  int findNumberAppearingOnce(vector<int> &nums) {
    auto val = 0;
    for (int i = 0; i < 32; i++) {
      auto cur = 0;
      for (auto &num : nums) {
        cur += (num >> i) & 1;
      }
      val |= (cur % 3) << i;
    }

    return val;
  }
};
