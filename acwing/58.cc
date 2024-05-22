#include "xxx.hpp"
#include <algorithm>
#include <vector>

class Solution {
public:
  int getUglyNumber(int n) {
    vector<int> ugs{1};
    auto i = 0;
    auto j = 0;
    auto k = 0;
    n -= 1;
    while (n--) {
      int cur = min({ugs[i] * 2, ugs[j] * 3, ugs[k] * 5});
      if (cur == ugs[i] * 2) {
        i += 1;
      }
      if (cur == ugs[j] * 3) {
        j += 1;
      }
      if (cur == ugs[k] * 5) {
        k += 1;
      }
      ugs.push_back(cur);
    }

    return ugs.back();
  }
};
