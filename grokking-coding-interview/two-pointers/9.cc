

#include <deque>
#include <iostream>
#include <vector>
using namespace std;

// Given an array of unsorted numbers and a target number, find all unique
// quadruplets in it, whose sum is equal to the target number.
class Solution {
 public:
  vector<vector<int>> four_sum(vector<int> &nums, int target) {
    sort(nums.begin(), nums.end());
    int sz = nums.size();
    vector<vector<int>> ret;
    for (int i = 0; i < sz - 3; i++) {
      // dedup
      if (i > 0 && nums[i] == nums[i - 1]) {
        continue;
      }
      for (int j = i + 1; j < sz - 2; j++) {
        if (j > i + 1 && nums[j] == nums[j - 1]) {
          continue;
        }
        two_sum(nums, i, j, target, j + 1, sz - 1, ret);
      }
    }
    return ret;
  }

 private:
  void two_sum(vector<int> &nums, int i, int j, int target, int left, int right,
               vector<vector<int>> &ret) {
    while (left < right) {
      int cur_sum = nums[i] + nums[j] + nums[left] + nums[right];
      if (cur_sum == target) {
        ret.push_back({nums[i], nums[j], nums[left], nums[right]});
        left++;
        right--;
        // dedup
        while (left < right && nums[left] == nums[left - 1]) {
          left++;
        }
        while (left < right && nums[right] == nums[right + 1]) {
          right--;
        }
      } else if (cur_sum < target) {
        // too small, need to get bigger
        left++;
      } else {
        // too big, shrink the big value
        right--;
      }
    }
  }
};

int main() {
  Solution so;
  vector<int> input{4, 1, 2, -1, 1, -3};
  auto numss = so.four_sum(input, 1);
  for (auto &nums : numss) {
    for (auto &num : nums) {
      cout << num << " ";
    }
    cout << endl;
  }
  cout << endl;
}

