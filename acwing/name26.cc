#include "xxx.hpp"

class Solution {
public:
  int NumberOf1(int n) {
    auto ret = 0;
    // 1 oneway
    // while (n) {
    //   ret += 1;
    //   int val = n & -n;
    //   n -= val;
    // }
    while (n) {
      ret += 1;
      n &= (n - 1);
    }
    return ret;
  }
};
