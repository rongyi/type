#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

class CylicSort {
 public:
  static void sort(vector<int> &nums) {
    int sz = nums.size();
    int i = 0;
    while (i < sz) {
      int expect_pos = nums[i] - 1;
      if (i == expect_pos) {
        i++;
        continue;
      }
      swap(nums[i], nums[expect_pos]);
    }
  }
};

class MissingOne {
 public:
  static int findMissing(vector<int> &nums) {
    int sz = nums.size();
    int i = 0;
    while (i < sz) {
      if (nums[i] < sz && i != nums[i]) {
        swap(nums[i], nums[nums[i]]);
      } else {
        i++;
      }
    }
    for (int i = 0; i < sz; i++) {
      if (nums[i] != i) {
        return i;
      }
    }

    return sz;
  }
};

int main() {
  // vector<int> input = {3, 1, 5, 4, 2};
  // CylicSort::sort(input);
  // for (auto &num : input) {
  // cout << num << endl;
  //}
  //
  vector<int> input{4, 0, 3, 1};
  cout << MissingOne::findMissing(input) << endl;
  input = {8, 3, 5, 2, 4, 6, 0, 1};
  cout << MissingOne::findMissing(input) << endl;
}

