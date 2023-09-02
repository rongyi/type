#include <bits/stdc++.h>
using namespace std;

// Given a string and a list of words, find all the starting indices of
// substrings in the given string that are a concatenation of all the given
// words exactly once without any overlapping of words. It is given that all
// words are of the same length.
vector<int> find_words_concanation(string &s, vector<string> &words) {
  int sz = s.size();
  // total words
  int wsz = words.size();
  int k = words[0].size();

  vector<int> ret;

  map<string, int> expected_count;
  for (auto &w : words) {
    expected_count[w]++;
  }

  for (int i = 0; i <= sz - wsz * k; i++) {
    map<string, int> cur_freq;
    int j = 0;
    for (; j < wsz; j++) {
      string cur = s.substr(i + j * k, k);
      if (!expected_count.count(cur)) {
        break;
      }
      cur_freq[cur]++;
      if (cur_freq[cur] > expected_count[cur]) {
        break;
      }
    }

    if (j == wsz) {
      ret.push_back(i);
    }
  }

  return ret;
}

int main() {
  string s{"catfoxcat"};
  vector<string> ps{{"cat"}, {"fox"}};
  // string s{"catcatfoxfox"};
  // vector<string> ps{{"cat"}, {"fox"}};
  auto r = find_words_concanation(s, ps);

  for (auto &i : r) {
    cout << i << " ";
  }
  cout << endl;
}

