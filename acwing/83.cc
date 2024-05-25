#include "xxx.hpp"
#include <cctype>
#include <limits>

class Solution {
public:
  int strToInt(string s) {
    auto sz = s.size();
    int i = 0;
    while (i < sz && s[i] == ' ') {
      i += 1;
    }
    if (i == sz) {
      return 0;
    }
    long long ret = 0;
    int sign = 1;
    if (s[i] == '+') {
      i += 1;
    } else if (s[i] == '-') {
      sign = -1;
      i += 1;
    }
    while (i < sz && isdigit(s[i])) {
      ret = ret * 10 + (s[i] - '0');
      if (ret * sign >= numeric_limits<int>::max()) {
        return numeric_limits<int>::max();
      } else if (ret * sign <= numeric_limits<int>::min()) {
        return numeric_limits<int>::min();
      }
      i += 1;
    }
    ret *= sign;
    if (ret < numeric_limits<int>::min()) {
      return numeric_limits<int>::min();
    } else if (ret > numeric_limits<int>::max()) {
      return numeric_limits<int>::max();
    }

    // cast to int
    return ret;
  }
};
