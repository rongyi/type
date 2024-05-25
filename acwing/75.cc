#include "xxx.hpp"
#include <vector>

class Solution {
public:
  vector<int> maxInWindows(vector<int> &nums, int k) {
    map<int, int> cnt;
    vector<int> ret;
    for (int j = 0, i = 0; j < nums.size(); j++) {
      cnt[nums[j]] += 1;
      if (j >= k) {
        cnt[nums[i]] -= 1;
        if (cnt[nums[i]] == 0) {
          cnt.erase(nums[i]);
        }
        i += 1;
      }

      if (j >= k - 1) {
        ret.push_back(cnt.rbegin()->first);
      }
    }
    return ret;
  }
};
