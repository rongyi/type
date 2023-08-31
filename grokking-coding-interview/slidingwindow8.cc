#include <bits/stdc++.h>
using namespace std;

// Given an array containing 0s and 1s, if you are allowed to replace no more
// than ‘k’ 0s with 1s, find the length of the longest contiguous subarray
// having all 1s.
int max_one_patch_zero(vector<int> &nums, int k) {
  int sz = nums.size();
  int ret = 0;

  int one_count = 0;
  // i...j
  for (int i = 0, j = 0; j < sz; j++) {
    one_count += (nums[j] == 1);
    if (j - i + 1 - one_count > k) {
      one_count -= (nums[i++] == 1);
    }
    ret = max(ret, j - i + 1);
  }

  return ret;
}

int main() {
  vector<int> input{0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1};
  auto r = max_one_patch_zero(input, 3);
  cout << r << endl;
}

