#include <algorithm>
#include <ios>
#include <iostream>

using namespace std;
using pii = pair<int, int>;
const int N = 2e5 + 10;
pii nums[N];

int main() {
  ios::sync_with_stdio(false);
  int n;
  cin >> n;
  for (int i = 0; i < n; i++) {
    int val;
    cin >> nums[i].first;
    nums[i].second = i;
  }

  sort(nums, nums + n);
  int ret = 1;
  int last = n + 1;
  // assume down first
  int dir = -1;

  for (int i = 0; i < n;) {
    int j = i + 1;
    while (j < n && nums[j].first == nums[i].first) {
      j += 1;
    }
    int min_idx = nums[i].second;
    int max_idx = nums[j - 1].second;

    // down case
    if (dir == -1) {
      if (last > max_idx) {
        last = min_idx;
      } else {
        dir = 1;
        last = max_idx;
      }
    } else {
      if (last < min_idx) {
        last = max_idx;
      } else {
        ret += 1;
        dir = -1;
        last = min_idx;
      }
    }

    i = j;
  }

  cout << ret << endl;

  return 0;
}
