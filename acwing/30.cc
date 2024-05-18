#include "xxx.hpp"
#include <utility>

class Solution {
public:
  void reOrderArray(vector<int> &a) {
    int i = 0;
    int j = a.size() - 1;
    int sz = a.size();
    while (i < j) {
      while (i < j && a[i] & 1) {
        i += 1;
      }
      while (i < j && (a[j] & 1) == 0) {
        j -= 1;
      }
      swap(a[i], a[j]);
    }
  }
};
