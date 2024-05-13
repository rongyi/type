#include "xxx.hpp"
#include <algorithm>

class Solution {
public:
  bool isPopOrder(vector<int> input, vector<int> out) {
    if (input.size() != out.size()) {
      return false;
    }

    // number shoulbe same
    auto cp_push = input;
    auto cp_pop = out;
    sort(cp_push.begin(), cp_push.end());
    sort(cp_pop.begin(), cp_pop.end());
    if (cp_push != cp_pop) {
      return false;
    }

    auto sz = input.size();
    map<int, int> idx_cache;
    for (int i = 0; i < sz; i++) {
      idx_cache[input[i]] = i;
    }
    map<int, bool> visited;
    vector<int> stk;

    for (auto &num : out) {
      auto pos = idx_cache[num];
      if (!stk.empty()) {
        auto top_idx = idx_cache[stk.back()];
        if (pos < top_idx) {
          return false;
        }
      }
      visited[num] = true;

      stk.clear();
      for (int i = 0; i < pos; i++) {
        if (!visited[input[i]]) {
          stk.push_back(input[i]);
        }
      }
    }

    return true;
  }
};
