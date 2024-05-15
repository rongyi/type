#include "xxx.hpp"

class Solution {
public:
  int NumberOf1(int n) {
    auto ret = 0;
    while (n) {
      ret += 1;
      int val = n & -n;
      n -= val;
    }
    return ret;
  }
};
