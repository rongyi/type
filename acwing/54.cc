#include "xxx.hpp"
#include <algorithm>
#include <string>

class Solution {
public:
  string printMinNumber(vector<int> &nums) {
    sort(nums.begin(), nums.end(), [](auto &l, auto &r) {
      if (to_string(l) + to_string(r) < to_string(r) + to_string(l)) {
        return true;
      }
      return false;
    });
    string res;
    for (auto num : nums) {
      res += to_string(num);
    }

    return res;
  }
};
