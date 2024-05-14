#include "xxx.hpp"
#include <algorithm>
#include <vector>

class Solution {
public:
  int longestSubstringWithoutDuplication(string s) {
    int sz = s.size();
    int ret = 0;
    auto i = 0;

    map<char, int> cnt;
    for (int j = 0; j < sz; j++) {
      cnt[s[j]] += 1;

      while (i < j && cnt.size() != (j - i + 1)) {
        cnt[s[i]] -= 1;
        if (cnt[s[i]] == 0) {
          cnt.erase(s[i]);
        }
        i += 1;
      }
      ret = max(ret, j - i + 1);
    }

    return ret;
  }
};

int main() {
  Solution so;
  auto cur = so.longestSubstringWithoutDuplication("iahysebzfgwskngepozq");
  cout << cur << endl;
}
