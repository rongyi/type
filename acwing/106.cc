#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
  int n;
  cin >> n;
  vector<int> nums(n, 0);
  for (int i = 0; i < n; i++) {
    cin >> nums[i];
  }
  sort(nums.begin(), nums.end());
  auto mid = n >> 1;
  auto sum = 0;
  for (int i = 0; i < n; i++) {
    sum += abs(nums[mid] - nums[i]);
  }
  cout << sum;
  return 0;
}
