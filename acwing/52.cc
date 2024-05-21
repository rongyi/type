#include "xxx.hpp"

using ll = long long;
class Solution {
public:
  int digitAtIndex(int serial_number) {
    ll d = 1;
    ll cnt = 9;
    ll base = 1;

    // 1..9 10
    // 10..99 90
    // 100..999 900
    // 1000..9999 9000
    while (serial_number > d * cnt) {
      // next level relative position
      serial_number -= d * cnt;
      d += 1;    // digit wide lenth, 3 -> 1 999 -> 3
      cnt *= 10; // total number for this wide, e.g. 10-99 have 90
      base *= 10;
    }
    // 1 index, make them 0 index
    auto num = base + (serial_number + d - 1) / d - 1;
    auto pos = serial_number % d;
    // the last postion
    if (pos == 0) {
      pos = d;
    }
    for (int i = 0; i < d - pos; i++) {
      num /= 10;
    }

    return num % 10;
  }
};
