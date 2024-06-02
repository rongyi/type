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
  int n;
  while (true) {
    cin >> n;
    if (n == 0) {
      break;
    }
    vector<int> nums(n, 0);
    for (int i = 0; i < n; i++) {
      cin >> nums[i];
    }
    cout << mergesort(nums, 0, nums.size() - 1) << endl;
  }
}
