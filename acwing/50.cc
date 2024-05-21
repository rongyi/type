#include "xxx.hpp"

class Solution {
public:
  int maxSubArray(vector<int> &nums) {
    int max_sum = nums[0];

    int acc = nums[0];

    for (int i = 1; i < nums.size(); i++) {
      // for every num we have two option:
      // 1. append to prev subarray
      // 2. standalone as the beginning of next subarray
      // the choice? make prev subarray larger else make it alone
      auto num = nums[i];
      acc += num;
      if (acc < num) {
        // newly start from num;
        acc = num;
      }
      max_sum = max(max_sum, acc);
    }

    return max_sum;
  }
};
