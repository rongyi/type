#include "xxx.hpp"
#include <algorithm>

class Solution {
public:
  string reverseWords(string s) {
    reverse(s.begin(), s.end());
    int sz = s.size();
    int i = 0;
    int j = 0;
    while (j < sz) {
      while (j < sz && s[j] != ' ') {
        j += 1;
      }
      if (j == sz) {
        break;
      }
      // now j stop at a space
      // rever range [i..j]
      reverse(s.begin() + i, s.begin() + j);
      i = j + 1;
      // skip space
      j += 1;
    }
    reverse(s.begin() + i, s.end());

    return s;
  }
};

int main() {
  Solution so;
  so.reverseWords("Being deeply loved by someone gives you strength, loving "
                  "someone deeply gives you courage.");
}
