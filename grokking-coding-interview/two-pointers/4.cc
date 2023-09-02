#include <bits/stdc++.h>
using namespace std;

// Given an array of unsorted numbers, find all unique triplets in it that add
// up to zero.
vector<vector<int>> three_sum(vector<int> &nums) {
  sort(nums.begin(), nums.end());
}

int main() {
  vector<int> input{-3, 0, 1, 2, -1, 1, -2};
  auto numss = three_sum(input);
  for (auto &nums : numss) {
    for (auto &num : nums) {
      cout << num << " ";
    }
  }
  cout << endl;
}

