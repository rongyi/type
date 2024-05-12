#include <vector>

using namespace std;

class Solution {
public:
  int duplicateInArray(vector<int> &nums) {
    auto l = 1;
    auto r = nums.size() - 1;
    // pigeon hole rule
    while (l < r) {
      auto mid = l + (r - l) / 2;
      auto s = 0;
      for (auto &num : nums) {
        s += num >= l && num <= mid;
      }
      // multiple value should be this part
      if (s > mid - l + 1) {
        r = mid;
      } else {
        l = mid + 1;
      }
    }

    return l;
  }
};
