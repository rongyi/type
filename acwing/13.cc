#include <vector>

using namespace std;

class Solution {
public:
  int duplicateInArray(vector<int> &nums) {
    auto sz = nums.size();
    for (auto &num : nums) {
      if (num < 0 || num >= sz) {
        return -1;
      }
    }
    for (int i = 0; i < sz; i++) {
      while (nums[i] != i) {
        // if we meet this number again, we encouter duplicate
        if (nums[i] == nums[nums[i]]) {
          return nums[i];
        }
        swap(nums[i], nums[nums[i]]);
      }
    }

    return -1;
  }
};
