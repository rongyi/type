#include "xxx.hpp"
#include <pthread.h>
#include <vector>

class Solution {
public:
  // prefix sum?
  vector<vector<int>> findContinuousSequence(int sum) {
    int end = (sum + 1) / 2;
    int i = 1;
    int j = 1;
    int prefix_sum = 0;
    vector<vector<int>> ret;
    while (j <= end) {
      prefix_sum += j;
      j += 1;
      while (prefix_sum > sum) {
        prefix_sum -= i;
        i += 1;
      }
      // must be two size
      if (prefix_sum == sum && i < j - 1) {
        vector<int> cur;
        // j already adanced
        for (int k = i; k < j; k++) {
          cur.push_back(k);
        }
        ret.push_back(cur);
      }
    }
    return ret;
  }
};

int main() {
  Solution so;
  so.findContinuousSequence(3);
}
