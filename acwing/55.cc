#include "xxx.hpp"
#include <vector>

class Solution {
public:
  int getTranslationCount(string s) {
    int sz = s.size();

    vector<int> cache(sz, -1);
    return dfs(s, sz, 0, cache);
  }

  int dfs(string &s, int sz, int i, vector<int> &cache) {
    if (i > sz) {
      return 0;
    }
    if (i == sz) {
      return 1;
    }
    if (cache[i] != -1) {
      return cache[i];
    }
    auto total = 0;
    total += dfs(s, sz, i + 1, cache);

    if (s[i] == '1' || (s[i] == '2' && i + 1 < sz &&
                        s[i + 1] <= '5' /* must >= '0' so omit here */)) {
      total += dfs(s, sz, i + 2, cache);
    }
    cache[i] = total;

    return cache[i];
  }
};

int main() {
  Solution so;

  auto cur = so.getTranslationCount("12258");
  cout << cur << endl;
}
