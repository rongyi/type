#include "xxx.hpp"
#include <algorithm>

class Solution {
public:
  bool isContinuous(vector<int> nums) {
    if (nums.empty()) {
      return false;
    }
    sort(nums.begin(), nums.end());
    int i = 0;
    int sz = nums.size();
    while (i < sz && nums[i] == 0) {
      i += 1;
    }
    for (int j = i + 1; j < sz; j++) {
      auto delta = nums[j] - nums[j - 1];
      if (delta == 0) {
        return false;
      }
      if (delta == 1) {
        continue;
      }
      if (delta - 1 > i) {
        return false;
      }
      //
      i -= delta - 1;
    }
    return true;
  }
};
