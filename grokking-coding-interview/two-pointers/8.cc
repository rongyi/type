

#include <deque>
#include <iostream>
#include <vector>
using namespace std;

// Given an array containing 0s, 1s and 2s, sort the array in-place. You should
// treat numbers of the array as objects, hence, we can’t count 0s, 1s, and 2s
// to recreate the array.

// The flag of the Netherlands consists of three colors: red, white and blue;
// and since our input array also consists of three different numbers that is
// why it is called Dutch National Flag problem.
class Solution {
 public:
  void sort(vector<int> &nums) {
    int sz = nums.size();
    int left = 0;
    int right = sz - 1;
    for (int i = 0; i <= right;) {
      if (nums[i] == 0) {
        swap(nums[i], nums[left]);
        left++;
      } else if (nums[i] == 1) {
        i++;
      } else {
        swap(nums[i], nums[right]);
        right--;
      }
    }
  }
};

int main() {
  Solution so;
  vector<int> input{1, 0, 2, 1, 0};
  so.sort(input);
  for (auto &num : input) {
    cout << num << " ";
  }
  cout << endl;
}

