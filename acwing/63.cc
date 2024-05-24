#include "xxx.hpp"

class Solution {
public:
  int getNumberOfK(vector<int> &nums, int k) {
    auto cnt = 0;
    for (auto &num : nums) {
      if (num == k) {
        cnt += 1;
      }
    }

    return cnt;
  }
};
