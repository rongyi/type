#include <iostream>
#include <vector>
using namespace std;

// Given an array of unsorted numbers and a target number, find a triplet in the
// array whose sum is as close to the target number as possible, return the sum
// of the triplet. If there are more than one such triplet, return the sum of
// the triplet with the smallest sum.
class Solution {
 public:
  int closest_three_sum(vector<int> &nums, int target) {
    int sz = nums.size();
    sort(nums.begin(), nums.end());
    int min_diff = numeric_limits<int>::max();

    for (int i = 0; i < sz - 2; i++) {
      int left = i + 1;
      int right = sz - 1;
      while (left < right) {
        int cur_sum = nums[i] + nums[left] + nums[right];
        int cur_diff = target - cur_sum;
        if (cur_diff == 0) {
          return cur_sum;
        }
        if (abs(cur_diff) < min_diff) {
          min_diff = abs(cur_diff);
        }
        if (cur_diff > 0) {
          left++;
        } else {
          right--;
        }
      }
    }

    return target - min_diff;
  }
};

int main() {
  vector<int> input{1, 0, 1, 1};
  Solution so;
  auto num = so.closest_three_sum(input, 100);
  cout << num << endl;
}

