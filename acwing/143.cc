#include <iostream>
#include <vector>

using namespace std;

string s;
int n = 0;

vector<int> genlps(string &s) {
  int sz = s.size();
  vector<int> lps(sz, 0);

  for (int i = 1, j = 0; i < sz; i++) {
    while (j > 0 && s[j] != s[i]) {
      // fallback
      j = lps[j - 1];
    }
    if (s[i] == s[j]) {
      lps[i] = ++j;
    }
  }

  return lps;
}

int main() {
  ios::sync_with_stdio(false);

  int round = 1;
  while (true) {
    cin >> n;
    if (n == 0) {
      break;
    }
    cin >> s;
    auto lps = genlps(s);

    cout << "Test case #" << round++ << endl;
    for (int i = 1; i < s.size(); i++) {
      auto len = i + 1;
      if ((len % (len - lps[i]) == 0) && len / (len - lps[i]) > 1) {
        cout << len << " " << len / (len - lps[i]) << endl;
      }
    }
    cout << endl;
  }

  return 0;
}
