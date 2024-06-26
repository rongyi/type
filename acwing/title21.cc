#include "xxx.hpp"

class Solution {
public:
  int Fibonacci(int n) {
    if (n == 0) {
      return 0;
    }
    int n1 = 1;
    int n2 = 1;
    if (n <= 2) {
      return 1;
    }
    int sum = 0;
    for (int i = 3; i <= n; i++) {
      sum = n1 + n2;
      n1 = n2;
      n2 = sum;
    }
    return sum;
  }
};
