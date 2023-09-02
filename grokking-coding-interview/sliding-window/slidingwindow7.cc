#include <bits/stdc++.h>
using namespace std;

// Given a string with lowercase letters only, if you are allowed to replace no
// more than ‘k’ letters with any letter, find the length of the longest
// substring having the same letters after replacement.
int longestsame_withpatch(string &input, int k) {
  int sz = input.size();
  map<char, int> freq;
  int cur_max_repeat = 0;
  int ret = 0;

  for (int i = 0, j = 0; j < sz; j++) {
    char c = input[j];
    freq[c]++;
    cur_max_repeat = max(cur_max_repeat, freq[c]);

    // edit too much
    if (j - i + 1 - cur_max_repeat > k) {
      freq[input[i++]]--;
    }
    ret = max(ret, j - i + 1);
  }

  return ret;
}

int main() {
  string input{"abccde"};
  auto r = longestsame_withpatch(input, 1);
  cout << r << endl;
}

