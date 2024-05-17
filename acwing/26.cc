#include "xxx.hpp"

class Solution {
public:
  double Power(double base, long long exponent) {
    if (base == 0) {
      return 0;
    }

    if (exponent == 0) {
      return 1;
    }
    if (exponent < 0) {
      return 1 / Power(base, -exponent);
    }
    double res = 1.0;
    for (long long i = exponent; i; i >>= 1) {
      if (i & 1) {
        res *= base;
      }
      base *= base;
    }
    return res;
  }
};
