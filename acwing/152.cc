#include <ios>
#include <iostream>
#include <vector>
using namespace std;

int main() {
  ios::sync_with_stdio(false);
  string s;
  cin >> s;

  int ret = 0;
  vector<int> stk;
  auto is_match = [](char l, char r) {
    return (l == '{' && r == '}') || (l == '[' && r == ']') ||
           (l == '(' && r == ')');
  };
  int next_valid = 0;
  for (int i = 0; i < s.size(); i++) {
    if (s[i] == '(' || s[i] == '[' || s[i] == '{') {
      stk.push_back(i);
    } else {
      // try to match
      if (stk.empty()) {
        next_valid = i + 1;
      } else {
        if (!is_match(s[stk.back()], s[i])) {
          stk.clear();
          next_valid = i + 1;
        } else {
          stk.pop_back();
          if (stk.empty()) {
            ret = max(ret, i - next_valid + 1);
          } else {
            ret = max(ret, i - stk.back());
          }
        }
      }
    }
  }

  cout << ret << endl;

  return 0;
}
