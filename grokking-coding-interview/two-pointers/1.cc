
#include <bits/stdc++.h>
using namespace std;

// Given an array of sorted numbers and a target sum, find a pair in the array
// whose sum is equal to the given target.

// Write a function to return the indices of the two numbers (i.e. the pair)
// such that they add up to the given target.
pair<int, int> sumtotarget(vector<int> &nums, int target) {
  int sz = nums.size();
  int i = 0;
  int j = sz - 1;
  while (i < j) {
    if (nums[i] + nums[j] == target) {
      return {i, j};
    } else if (nums[i] + nums[j] > target) {
      j--;
    } else {
      i++;
    }
  }

  return {-1, -1};
}

int main() {
  vector<int> input{1, 2, 3, 4, 6};
  auto p = sumtotarget(input, 6);
  cout << p.first << " " << p.second << endl;
}

