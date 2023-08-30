#include <bits/stdc++.h>
using namespace std;

// Given a string, find the length of the longest substring in it with no more
// than K distinct characters.
// i    j
//        ^ > k
int find_length(string s, int k) {
  vector<int> count(26, 0);
  int ret = 0;
  int sz = s.size();
  int win_char_cnt = 0;

  // i    j
  for (int i = 0, j = 0; j < sz; j++) {
    win_char_cnt += (++count[s[j] - 'a'] == 1);
    while (win_char_cnt > k) {
      win_char_cnt -= (--count[s[i] - 'a'] == 0);
      i++;
    }
    ret = max(ret, j - i + 1);
  }

  return ret;
}

int main() {
  auto r = find_length("cbbebi", 3);
  cout << r << endl;
}

