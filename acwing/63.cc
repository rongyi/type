#include "xxx.hpp"

class Solution {
public:
  char firstNotRepeatingChar(string s) {
    auto sz = s.size();
    map<int, int> cnt;
    for (auto c : s) {
      cnt[c] += 1;
    }
    for (auto c : s) {
      if (cnt[c] == 1) {
        return c;
      }
    }

    return '#';
  }
};
