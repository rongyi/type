#include <iostream>
#include <vector>
using namespace std;

long long mergesort(vector<int> &nums, int l, int r) {
  if (l >= r) {
    return 0;
  }
  auto mid = l + (r - l) / 2;
  auto ret = mergesort(nums, l, mid) + mergesort(nums, mid + 1, r);
  int i = l;
  int j = mid + 1;
  int k = 0;
  vector<int> tmp(r - l + 1, 0);
  while (i <= mid && j <= r) {
    if (nums[i] <= nums[j]) {
      tmp[k++] = nums[i++];
    } else {
      tmp[k++] = nums[j++];
      ret += (mid - i + 1);
    }
  }
  while (i <= mid) {
    tmp[k++] = nums[i++];
  }
  while (j <= r) {
    tmp[k++] = nums[j++];
  }
  for (int i = 0, j = l; i < tmp.size(); i++) {
    nums[j++] = tmp[i];
  }

  return ret;
}

int main() {
  vector<int> start_stat;
  vector<int> end_stat;
  int n;
  while (cin >> n) {
    start_stat.clear();
    start_stat.reserve(n * n);
    end_stat.clear();
    end_stat.reserve(n * n);
    for (int i = 0; i < n * n; i++) {
      int val = 0;
      cin >> val;
      if (val != 0) {
        start_stat.push_back(val);
      }
    }

    for (int i = 0; i < n * n; i++) {
      int val = 0;
      cin >> val;
      if (val != 0) {
        end_stat.push_back(val);
      }
    }
    auto res1 = mergesort(start_stat, 0, start_stat.size() - 1);
    auto res2 = mergesort(end_stat, 0, end_stat.size() - 1);
    if ((res1 & 1) == (res2 & 1)) {
      cout << "TAK" << endl;
    } else {
      cout << "NIE" << endl;
    }
  }
  return 0;
}
