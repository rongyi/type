#include "xxx.hpp"

class Solution {
public:
  // until there is no carry
  int add(int num1, int num2) {
    // 1、两个整数做异或^，得到各位相加不进位的运算结果；
    // 2、两个整数做与&，然后再左移一位，即得到进位的运算结果；
    // 3、将上面两个结果相加，即重复步骤1,2，直至进位的运算结果为0；
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
