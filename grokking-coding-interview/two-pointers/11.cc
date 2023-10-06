

#include <deque>
#include <iostream>
#include <vector>
using namespace std;

// Given an array, find the length of the smallest subarray in it which when
// sorted will sort the whole array.
class Solution {
 public:
  int min_window_sort(vector<int> &nums) {
    // [l, r]
    // min(l..=r)
    // max(l..=r)
    int sz = nums.size();
    int l = 0;
    int r = sz - 1;
    while (l < sz - 1 && nums[l] <= nums[l + 1]) {
      l++;
    }
    // sorted
    if (l == sz - 1) {
      return 0;
    }
    while (r > 0 && nums[r] >= nums[r - 1]) {
      r--;
    }
    // [l, r]
    int range_min = numeric_limits<int>::max();
    int range_max = numeric_limits<int>::min();
    for (int i = l; i <= r; i++) {
      range_min = min(range_min, nums[i]);
      range_max = max(range_max, nums[i]);
    }
    // extend l
    while (l > 0 && nums[l - 1] > range_min) {
      l--;
    }
    while (r < sz - 1 && nums[r + 1] < range_max) {
      r++;
    }
    return r - l + 1;
  }
};

int main() {
  Solution so;
  vector<int> input{1, 2, 5, 3, 7, 10, 9, 12};
  auto sz = so.min_window_sort(input);

  cout << sz << endl;
}

