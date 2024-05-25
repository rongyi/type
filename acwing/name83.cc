#include <algorithm>
#include <vector>

using namespace std;

class Solution {
public:
  int maxDiff(vector<int> &nums) {
    if (nums.empty()) {
      return 0;
    }
    auto sz = nums.size();
    auto prev_min = nums[0];
    auto max_profit = 0;
    for (int i = 1; i < sz; i++) {
      max_profit = max(max_profit, nums[i] - prev_min);
      max_profit = max(max_profit, 0);
      prev_min = min(prev_min, nums[i]);
    }

    return max_profit;
  }
};
