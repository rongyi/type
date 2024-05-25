#include "xxx.hpp"

class Solution {
public:
  // until there is no carry
  int add(int num1, int num2) {
    while (num2) {
      auto sum = num1 ^ num2;
      auto carry = (num1 & num2) << 1;
      num1 = sum;
      num2 = carry;
    }
    return num1;
  }
};

int main() {
  Solution so;
  so.add(1, 2);
}
