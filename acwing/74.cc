#include "xxx.hpp"

class Solution {
public:
  string leftRotateString(string str, int n) {
    if (str.empty()) {
      return "";
    }
    int sz = str.size();
    n = n % sz;
    if (n == 0) {
      return str;
    }
    auto head = str.substr(0, n);
    auto tail = str.substr(n);
    return tail + head;
  }
};
