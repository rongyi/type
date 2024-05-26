#include "xxx.hpp"

class Solution {
public:
// HELP
int numberOf1Between1AndN_Solution(int n) {
    int base = 1;
    int ret = 0;
    while (n / base != 0) {
      // e.g. 389
      //       ^  --> base = 10
      // high = 3
      // low = 9
      // cur = 8
      int high = n / (base * 10);
      int low = n - (n / base) * base;
      int cur = (n / base) % 10;
      // e.g. 389
      //       ^
      // 去统计8这个位置上有多少个1，
      // 1. 看三位数： high * base
      //    比如这个例子三位高位就有三个，1 2 3
      //    ，那么这个位置上固定好1以后，低位还可以从0-9，
      //    所以乘以base是这个意思，
      // 2. 高位没有了，从两位数看起
      //    1. 如果为0， 没可能了，不考虑
      //    2. 如果为1，那么后面低位还可以从0->low 也即 low + 1个数字
      //    3. 如果>1, 那么固定1在这一位之后，低位还可以从0-9 10个可能
      if (cur == 0) {
        ret += high * base;
      } else if (cur == 1) {
        ret += high * base + low + 1;
      } else {
        ret += high * base + base;
      }

      base *= 10;
    }
    return ret;
  }
};
