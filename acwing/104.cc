#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

bool check(vector<int> &nums, double avg, int gap) {
  int sz = nums.size();
  vector<double> prefix(sz, 0);
  for (int i = 1; i < sz; i++) {
    prefix[i] = prefix[i - 1] + nums[i] - avg;
  }
  double min_prev = 0.0;
  for (int i = 0, j = gap; j < sz; j++, i++) {
    min_prev = min(min_prev, prefix[i]);
    if (prefix[j] - min_prev >= 0) {
      return true;
    }
  }
  return false;
}

int main() {
  int sz, gap;
  cin >> sz >> gap;
  vector<int> nums(sz + 1, 0);
  double l = 0;
  double r = 0;
  for (int i = 0; i < sz; i++) {
    cin >> nums[i + 1];
    r = max(r, (double)nums[i + 1]);
  }
  // binary search
  while (r - l > 1e-5) {
    auto mid = l + (r - l) / 2.0;
    if (check(nums, mid, gap)) {
      l = mid;
    } else {
      r = mid;
    }
  }
  cout << (int)(r * 1000) << endl;
  return 0;
}
