#include "xxx.hpp"

class Solution {
public:
  int getNumberSameAsIndex(vector<int> &nums) {
    for (int i = 0; i < nums.size(); i++) {
      if (nums[i] == i) {
        return i;
      }
    }
    return -1;
  }
};
