#include "xxx.hpp"
#include <vector>

class Solution {
public:
  bool searchArray(vector<vector<int>> array, int target) {
    int m = array.size();
    if (m == 0) {
      return false;
    }
    int n = array[0].size();
    if (n == 0) {
      return false;
    }
    int l = array[0][0];
    int r = array[m - 1][n - 1];
    if (target < l || target > r) {
      return false;
    }

    int i = m - 1;
    int j = 0;
    while (i >= 0 && j < n) {
      // this value is the biggest in this column
      // and smallest in this row
      auto val = array[i][j];
      if (target < val) {
        i -= 1;
      } else if (target == val) {
        return true;
      } else {
        j += 1;
      }
    }

    return false;
  }
};

int main() {
  Solution so;
  vector<vector<int>> input{
      {1, 2, 8, 9}, {2, 4, 9, 12}, {4, 7, 10, 13}, {6, 8, 11, 15}};
  so.searchArray(input, 7);
}
