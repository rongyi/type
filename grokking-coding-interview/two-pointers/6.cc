
#include <iostream>
#include <vector>
using namespace std;

// Given an array arr of unsorted numbers and a target sum, count all triplets
// in it such that arr[i] + arr[j] + arr[k] < target where i, j, and k are three
// different indices. Write a function to return the count of such triplets.
class Solution {
 public:
  // x + y + z < target
  //     y + z < target - x
  int total_triplet_less_than_target(vector<int> &nums, int target) {
    sort(nums.begin(), nums.end());
    int sz = nums.size();
    int count = 0;
    for (int i = 0; i < sz - 2; i++) {
      count += two_smaller(nums, target - nums[i], i + 1, sz - 1);
    }

    return count;
  }

 private:
  int two_smaller(vector<int> &nums, int target, int left, int right) {
    int count = 0;
    while (left < right) {
      int sum = nums[left] + nums[right];
      // [..... l           r....]
      // l + r < target
      //        l........r-1
      //        nums[l] + nums[r - 1]
      if (sum < target) {
        count += right - left;
        left++;
      } else {
        right--;
      }
    }

    return count;
  }
};

int main() {
  vector<int> input{-1, 4, 2, 1, 3};
  Solution so;
  auto num = so.total_triplet_less_than_target(input, 5);
  cout << num << endl;
}

