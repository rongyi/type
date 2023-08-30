#include <bits/stdc++.h>
using namespace std;

vector<double> average_of_window(vector<int> &nums, int k) {
  int sz = nums.size();
  vector<double> ret(sz - k + 1);
  double win_sum = 0;
  // i for head, j for tail
  for (int i = 0, j = 0; i < sz; i++) {
    win_sum += nums[i];
    if (i >= k - 1) {
      ret[j] = win_sum / k;
      // for next round
      win_sum -= nums[j++];
    }
  }
  return ret;
}

int main() {
  vector<int> input{1, 3, 2, 6, -1, 4, 1, 8, 2};
  auto avgs = average_of_window(input, 5);
  for (auto &avg : avgs) {
    cout << avg << " ";
  }
  cout << endl;
}

