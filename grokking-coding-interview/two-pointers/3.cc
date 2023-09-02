#include <bits/stdc++.h>
using namespace std;

// Given a sorted array, create a new array containing squares of all the number
// of the input array in the sorted order.
// [-4, -2, 3]
vector<int> square_nums(vector<int> &nums) {
  int sz = nums.size();
  int i = 0;
  int j = sz - 1;

  vector<int> ret(sz, 0);
  int k = sz - 1;
  while (i < j) {
    auto sqi = nums[i] * nums[i];
    auto sqj = nums[j] * nums[j];
    if (sqi > sqj) {
      ret[k--] = sqi;
      i++;
    } else {
      ret[k--] = sqj;
      j--;
    }
  }
  return ret;
}

int main() {
  vector<int> input{-3, -1, 0, 1, 2};
  auto nums = square_nums(input);
  for (auto &num : nums) {
    cout << num << " ";
  }
  cout << endl;
}

