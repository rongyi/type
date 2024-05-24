#include "xxx.hpp"

class Solution {
public:
  int getMissingNumber(vector<int> &nums) {
    int i = 0;
    for (int j = 0; j < nums.size(); j++, i++) {
      if (nums[j] != i) {
        return i;
      }
    }
    return nums.size();
  }
};
