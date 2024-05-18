#include "xxx.hpp"

class Solution {
public:
  bool isMatch(string s, string p) {
    int m = s.size();
    int n = p.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, -1));

    return recur(s, p, 0, 0, dp);
  }

  int recur(string &s, string &p, int i, int j, vector<vector<int>> &dp) {
    if (dp[i][j] != -1) {
      return dp[i][j];
    }

    if (j == p.size()) {
      return dp[i][j] = i == s.size();
    }
    auto ret = 0;
    auto first_match = (i < s.size()) && (s[i] == p[j] || p[j] == '.');

    // 1. a* .* case
    //    + just ignore this two char to match [i][j + 2]
    //    + or match one or more, more case is contained
    //      in recursive process
    //      so just write [i+1][j]
    if (j + 1 < p.size() && p[j + 1] == '*') {
      ret = recur(s, p, i, j + 2, dp) ||
            (first_match && recur(s, p, i + 1, j, dp));

    } else {
      // normal one by one case
      ret = first_match && recur(s, p, i + 1, j + 1, dp);
    }

    dp[i][j] = ret;

    return dp[i][j];
  }
};
