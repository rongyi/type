#include <bits/stdc++.h>
using namespace std;

// Given a string and a pattern, find the smallest substring in the given string
// which has all the characters of the given pattern.
string shorted_substr_contains(string &s, string &pattern) {
  vector<int> expected_chars(26, 0);
  int expected_total_chars = 0;
  for (auto &c : pattern) {
    if (++expected_chars[c - 'a'] == 1) {
      expected_total_chars++;
    }
  }
  int min_len = s.size() + 1;
  int min_start = -1;

  int win_matched_chars = 0;
  // i..j
  for (int i = 0, j = 0; j < s.size(); j++) {
    if (--expected_chars[s[j] - 'a'] == 0) {
      win_matched_chars++;
    }

    while (win_matched_chars == expected_total_chars) {
      if (j - i + 1 < min_len) {
        min_len = j - i + 1;
        min_start = i;
      }
      if (++expected_chars[s[i++] - 'a'] == 1) {
        win_matched_chars--;
      }
    }
  }

  if (min_start == -1) {
    return "";
  }
  return s.substr(min_start, min_len);
}

int main() {
  // string s{"odicf"};
  // string p{"dc"};
  string s{"abdabca"};
  string p{"abc"};
  auto r = shorted_substr_contains(s, p);

  cout << r << endl;
}

