#include <bits/stdc++.h>
using namespace std;

// Given an array of characters where each character represents a fruit tree,
// you are given two baskets and your goal is to put maximum number of fruits in
// each basket. The only restriction is that each basket can have only one type
// of fruit.

// You can start with any tree, but once you have started you can’t skip a tree.
// You will pick one fruit from each tree until you cannot, i.e., you will stop
// when you have to pick from a third fruit type.

// Write a function to return the maximum number of fruits in both the baskets.
int find_length(vector<char> &fruits) {
  vector<int> count(26, 0);
  int win_char_cnt = 0;
  int sz = fruits.size();
  int ret = 0;

  for (int i = 0, j = 0; j < sz; j++) {
    win_char_cnt += (++count[fruits[j] - 'a'] == 1);
    while (win_char_cnt > 2) {
      win_char_cnt -= (--count[fruits[i] - 'a'] == 0);
      i++;
    }
    ret = max(ret, j - i + 1);
  }

  return ret;
}

int main() {
  vector<char> input{'a', 'b', 'c', 'b', 'b', 'c'};
  auto r = find_length(input);
  cout << r << endl;
}

