#include "xxx.hpp"

class Solution {
public:
  int getSum(int n) {
    auto sum = n;

    (n > 0) && (sum += getSum(n - 1));

    return sum;
  }
};
