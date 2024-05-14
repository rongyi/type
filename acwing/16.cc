#include "xxx.hpp"

class Solution {
public:
  string replaceSpaces(string &str) {
    string ret;
    for (auto c : str) {
      if (c == ' ') {
        ret.push_back('%');
        ret.push_back('2');
        ret.push_back('0');
      } else {
        ret.push_back(c);
      }
    }

    return ret;
  }
};
