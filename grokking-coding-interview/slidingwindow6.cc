#include <bits/stdc++.h>
using namespace std;

// Given a string, find the length of the longest substring which has no
// repeating characters.
int max_no_repeating_substr(string &input) {
  int sz = input.size();
  map<int, int> index;

  int ret = 0;
  // i ... j
  for (int i = 0, j = 0; j < sz; j++) {
    char c = input[j];
    if (index.count(c)) {
      i = max(i, index[c] + 1);
    }
    index[c] = j;
    ret = max(ret, j - i + 1);
  }
  return ret;
}

int main() {
  string input{"abcde"};
  auto r = max_no_repeating_substr(input);
  cout << r << endl;
}

