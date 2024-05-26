#include "xxx.hpp"
#include <vector>

class Solution {
public:
  int inversePairs(vector<int> &nums) {
    int sz = nums.size();
    merge_sort(nums, 0, sz - 1);

    // for (auto &num : nums) {
    //   cout << num << endl;
    // }

    return total_reverse;
  }
  void merge_sort(vector<int> &nums, int l, int r) {
    if (l >= r) {
      return;
    }
    auto mid = l + (r - l) / 2;
    merge_sort(nums, l, mid);
    merge_sort(nums, mid + 1, r);

    do_merge(nums, l, mid, r);
  }
  void do_merge(vector<int> &nums, int l, int mid, int r) {
    vector<int> tmp(r - l + 1, 0);
    // calculate reverse pairs
    for (int i = l; i <= mid; i++) {
      int j = r;
      while (j >= mid + 1 && nums[i] <= nums[j]) {
        j -= 1;
      }
      // int cur_reverse = (mid - i + 1) * (j - mid);
      int cur_reverse = (j - mid);
      // cout << "cur reverse: " << cur_reverse << endl;
      total_reverse += cur_reverse;
    }

    int i = l;
    int j = mid + 1;
    int k = 0;
    while (i <= mid && j <= r) {
      if (nums[i] <= nums[j]) {
        tmp[k++] = nums[i++];
      } else {
        tmp[k++] = nums[j++];
      }
    }
    while (i <= mid) {
      tmp[k++] = nums[i++];
    }
    while (j <= r) {
      tmp[k++] = nums[j++];
    }
    // cout << "in sorted" << endl;
    // for (auto &num : tmp) {
    //   cout << num << " ";
    // }
    // cout << endl;
    for (int i = l, k = 0; i <= r; i++) {
      nums[i] = tmp[k++];
    }
  }

private:
  int total_reverse = 0;
};

int main() {
  Solution so;
  vector<int> input{1, 2, 3, 4, 5, 6, 0};
  so.inversePairs(input);
}
