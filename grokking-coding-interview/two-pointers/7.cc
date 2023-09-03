

#include <deque>
#include <iostream>
#include <vector>
using namespace std;

class Solution {
 public:
  vector<vector<int>> total_triplet_less_than_target(vector<int> &nums,
                                                     int target) {
    int sz = nums.size();
    vector<vector<int>> ret;

    int product = 1;
    // i..j
    for (int i = 0, j = 0; j < sz; j++) {
      product *= nums[j];
      while (product >= target) {
        product /= nums[i++];
      }

      // [i, j]
      deque<int> q;
      for (int k = j; k >= i; k--) {
        vector<int> cur;
        q.push_front(nums[k]);

        move(q.begin(), q.end(), back_inserter(cur));
        ret.push_back(cur);
      }
    }

    return ret;
  }
};

int main() {
  Solution so;
  vector<int> input{2, 5, 3, 10};
  auto numss = so.total_triplet_less_than_target(input, 30);
  for (auto &nums : numss) {
    for (auto &num : nums) {
      cout << num << " ";
    }
    cout << endl;
  }
  cout << endl;
}

