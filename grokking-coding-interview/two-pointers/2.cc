

#include <bits/stdc++.h>
using namespace std;

// Given an array of sorted numbers, remove all duplicates from it. You should
// not use any extra space; after removing the duplicates in-place return the
// new length of the array.
int dedup(vector<int> &nums) {
  int sz = nums.size();
  // next dedup num postion
  int i = 1;
  for (int j = 1; j < sz; j++) {
    if (nums[j] != nums[i - 1]) {
      nums[i++] = nums[j];
    }
  }

  return i;
}

int main() {
  vector<int> input{1, 1, 2, 3, 3};
  int sz = dedup(input);
  cout << sz << endl;
}

