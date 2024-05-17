#include "xxx.hpp"
#include <vector>

class Solution {
public:
  int findMin(vector<int> &nums) {
    if (nums.empty()) {
      return -1;
    }
    int j = nums.size() - 1;
    while (j > 0 && nums[j] == nums[0]) {
      j -= 1;
    }
    // mono non decrease
    if (nums[j] >= nums[0]) {
      return nums[0];
    }
    // now we have left size >= minnumber
    // right side
    int l = 0;
    int r = j;
    while (l < r) {
      auto mid = l + (r - l) / 2;
      if (nums[mid] >= nums[0]) {
        l = mid + 1;
      } else {
        r = mid;
      }
    }
    return nums[l];
  }
};

int main() {
  Solution so;
  vector<int> input{10, 10, 1, 2, 3, 4, 4, 5, 7, 8};
  so.findMin(input);
}
