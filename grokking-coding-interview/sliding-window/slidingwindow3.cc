#include <bits/stdc++.h>
using namespace std;

// Given an array of positive numbers and a positive number ‘S’, find the length
// of the smallest contiguous subarray whose sum is greater than or equal to
// ‘S’. Return 0, if no such subarray exists.
// 2 1 5 2 3 2
//   ^ ^
int min_subarray(vector<int> &nums, int target) {
  int sz = nums.size();
  int ret = numeric_limits<int>::max();

  int win_sum = 0;
  // i ... j
  for (int i = 0, j = 0; j < sz; j++) {
    win_sum += nums[j];
    while (win_sum >= target) {
      ret = min(ret, j - i + 1);
      win_sum -= nums[i++];
    }
  }
  if (ret == numeric_limits<int>::max()) {
    return -1;
  }
  return ret;
}

int main() {
  vector<int> input{2, 1, 5, 2, 3, 2};
  auto max_sum = min_subarray(input, 15);
  cout << max_sum << endl;
}

