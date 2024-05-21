#include "xxx.hpp"

class Solution {
public:
  int moreThanHalfNum_Solution(vector<int> &nums) {
    int cnt = 1;
    int val = nums[0];
    for (int i = 1; i < nums.size(); i++) {
      if (nums[i] == val) {
        cnt += 1;
      } else {
        cnt -= 1;
      }
      if (cnt == 0) {
        val = nums[i];
        cnt = 1;
      }
    }

    return val;
  }
};
