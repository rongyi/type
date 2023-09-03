#include <iostream>
#include <vector>
using namespace std;

class Solution {
 public:
  // Given an array of unsorted numbers, find all unique triplets in it that add
  // up to zero.
  vector<vector<int>> three_sum(vector<int> &nums) {
    int sz = nums.size();
    sort(nums.begin(), nums.end());
    vector<vector<int>> ret;
    for (int i = 0; i < sz; i++) {
      if (i > 0 && nums[i] == nums[i - 1]) {
        continue;
      }
      two_sum(nums, -nums[i], i + 1, sz - 1, ret);
    }
    return ret;
  }

 private:
  void two_sum(vector<int> &nums, int target, int left, int right,
               vector<vector<int>> &ret) {
    while (left < right) {
      int sum = nums[left] + nums[right];
      if (sum == target) {
        ret.push_back({-target, nums[left], nums[right]});
        left++;
        right--;
        while (left < right && nums[left] == nums[left - 1]) {
          left++;
        }
        while (left < right && nums[right] == nums[right + 1]) {
          right--;
        }
      } else if (sum < target) {
        left++;
      } else {
        right--;
      }
    }
  }
};

int main() {
  vector<int> input{-3, 0, 1, 2, -1, 1, -2};
  Solution so;
  auto numss = so.three_sum(input);
  for (auto &nums : numss) {
    for (auto &num : nums) {
      cout << num << " ";
    }
    cout << endl;
  }
  cout << endl;
}

