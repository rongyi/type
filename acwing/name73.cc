#include <vector>

using namespace std;

class Solution {
public:
  vector<int> findNumsAppearOnce(vector<int> &nums) {
    auto two_xor = 0;
    for (auto &num : nums) {
      two_xor ^= num;
    }
    two_xor &= -two_xor;
    vector<int> a;
    vector<int> b;

    auto num1 = 0;
    auto num2 = 0;
    for (auto &num : nums) {
      if ((num & two_xor) != 0) {
        num1 ^= num;
      } else {
        num2 ^= num;
      }
    }

    return {num1, num2};
  }
};
