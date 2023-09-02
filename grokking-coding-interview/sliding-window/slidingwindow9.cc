#include <bits/stdc++.h>
using namespace std;

// Given a string and a pattern, find out if the string contains any permutation
// of the pattern.
// abcde  dce
bool is_permutation_exist(string &s, string &pattern) {
  vector<int> expected_chars(26, 0);
  int expected_total_chars = 0;
  for (auto &c : pattern) {
    if (++expected_chars[c - 'a'] == 1) {
      expected_total_chars++;
    }
  }

  int k = pattern.size();
  int win_chars = 0;
  // i..j
  for (int i = 0, j = 0; j < s.size(); j++) {
    if (--expected_chars[s[j] - 'a'] == 0) {
      win_chars++;
    }
    if (win_chars == expected_total_chars) {
      return true;
    }
    if (i >= k - 1) {
      if (++expected_chars[s[i++] - 'a'] == 1) {
        win_chars--;
      }
    }
  }

  return false;
}

int main() {
  // string s{"odicf"};
  // string p{"dc"};
  string s{"bcdxabcdy"};
  string p{"bcdyabcdx"};
  auto r = is_permutation_exist(s, p);

  cout << r << endl;
}

