#include "xxx.hpp"
#include <algorithm>

class Solution {
public:
  vector<int> findNumbersWithSum(vector<int> &nums, int target) {
    sort(nums.begin(), nums.end());
    int i = 0;
    int j = nums.size() - 1;
    while (i < j) {
      auto cur_sum = nums[i] + nums[j];
      if (cur_sum == target) {
        return {nums[i], nums[j]};
      } else if (cur_sum < target) {
        i += 1;
      } else {
        j -= 1;
      }
    }
    return {-1, -1};
  }
};
