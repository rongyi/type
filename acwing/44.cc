#include "xxx.hpp"
#include <vector>

class Solution {
public:
  bool verifySequenceOfBST(vector<int> nums) {
    int sz = nums.size();

    return dfs(nums, 0, sz - 1);
  }
  bool dfs(vector<int> &nums, int l, int r) {
    if (l >= r) {
      return true;
    }
    auto root = nums[r];
    int k = l;
    while (nums[k] < root) {
      k += 1;
    }
    // from k -> r - 1 should all > root
    for (int j = k; j < r; j++) {
      if (nums[j] < root) {
        return false;
      }
    }
    return dfs(nums, l, k - 1) && dfs(nums, k, r - 1);
  }
};
