

#include <deque>
#include <iostream>
#include <vector>
using namespace std;

// Given two strings containing backspaces (identified by the character ‘#’),
// check if the two strings are equal.

class Solution {
 public:
  bool str_same_with_backspace(string &s1, string &s2) {
    string cleans1;
    string cleans2;
    auto clean = [&](string &out, string &s) {
      for (auto &c : s) {
        if (c == '#') {
          if (!out.empty()) {
            out.pop_back();
          }
        } else {
          out.push_back(c);
        }
      }
    };
    clean(cleans1, s1);
    clean(cleans2, s2);

    return cleans1 == cleans2;
  }
};

int main() {
  Solution so;
  string s1{"xp#"};
  string s2{"xyz##"};
  auto same = so.str_same_with_backspace(s1, s2);

  cout << same << endl;
}

