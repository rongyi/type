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
      if (nums[i] < sz && nums[i] != nums[nums[i]]) {
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

class AllMissingNumbers {
 public:
  static vector<int> allMissingNumber(vector<int> &nums) {
    int sz = nums.size();
    int i = 0;
    while (i < sz) {
      // current value should at postion: expect_pos
      if (nums[i] != nums[nums[i] - 1]) {
        swap(nums[i], nums[nums[i] - 1]);
      } else {
        i++;
      }
    }
    vector<int> ret;
    for (int i = 0; i < sz; i++) {
      if (nums[i] != i + 1) {
        ret.push_back(i + 1);
      }
    }

    return ret;
  }
};

int main() {
  // vector<int> input = {3, 1, 5, 4, 2};
  // CylicSort::sort(input);
  // for (auto &num : input) {
  // cout << num << endl;
  //}
  //
  // vector<int> input{4, 0, 3, 1};
  // cout << MissingOne::findMissing(input) << endl;
  // input = {8, 3, 5, 2, 4, 6, 0, 1};
  // cout << MissingOne::findMissing(input) << endl;
  //
  vector<int> input{2, 3, 1, 8, 2, 3, 5, 1};
  auto missing = AllMissingNumbers::allMissingNumber(input);
  for (auto m : missing) {
    cout << m << endl;
  }
}

