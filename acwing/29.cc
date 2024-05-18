#include "xxx.hpp"

class Solution {
public:
  bool isNumber(string s) {
    int sz = s.size();
    int j = sz - 1;
    int i = 0;
    while (j >= 0 && s[j] == ' ') {
      j -= 1;
    }
    if (j < i) {
      return false;
    }
    sz = j + 1;

    auto is_num = false;
    // ignore leading spaces
    while (i < sz && s[i] == ' ') {
      i += 1;
    }
    if (i == sz) {
      return false;
    }
    // ignore prefix sign
    if (s[i] == '+' || s[i] == '-') {
      i += 1;
    }

    if (i == sz) {
      return false;
    }
    // all number
    while (i < sz && isdigit(s[i])) {
      is_num = true;
      i += 1;
    }
    if (i == sz) {
      return true;
    }
    if (s[i] == '.') {
      i += 1;
    }
    // decimal part
    while (i < sz && isdigit(s[i])) {
      is_num = true;
      i += 1;
    }
    if (i < sz && is_num && (s[i] == 'e' || s[i] == 'E')) {
      i += 1;
      is_num = false;
      if (i == sz) {
        return false;
      }
      if (s[i] == '-' || s[i] == '+') {
        i += 1;
      }
      if (i == sz) {
        return false;
      }
      while (i < sz && isdigit(s[i])) {
        i += 1;
        is_num = true;
      }
    }

    return i == sz && is_num;
  }
};
