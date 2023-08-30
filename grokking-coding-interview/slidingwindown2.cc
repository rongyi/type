#include <bits/stdc++.h>
using namespace std;

int max_sum_subarray(vector<int> &nums, int k) {
  int ret = numeric_limits<int>::min();
  int sz = nums.size();
  // i is head, j is tail
  int win_sum = 0;
  for (int i = 0, j = 0; i < sz; i++) {
    win_sum += nums[i];
    if (i >= k - 1) {
      ret = max(ret, win_sum);
      win_sum -= nums[j++];
    }
  }

  return ret;
}

int main() {
  vector<int> input{2, 1, 5, 1, 3, 7};
  auto max_sum = max_sum_subarray(input, 3);
  cout << max_sum << endl;
}

