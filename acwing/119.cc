#include <iostream>
#include <map>
#include <vector>

using namespace std;

const int n = 14;
vector<vector<int>> cards(14);
vector<int> values(n);

int main() {
  // do as they say
  map<char, int> index;
  index['A'] = 1;
  for (char c = '2', i = 2; c <= '9'; c++) {
    index[c] = i++;
  }
  index['0'] = 10;
  index['J'] = 11;
  index['Q'] = 12;
  index['K'] = 13;

  for (int i = 1; i <= 13; i++) {
    for (int j = 0; j < 4; j++) {
      char c;
      cin >> c;
      cards[i].push_back(index[c]);
    }
  }

  int cnt = 0;
  for (int i = 0; i < 4; i++) {
    char cur = cards[13][i];
    while (cur != 13) {
      values[cur] += 1;
      if (values[cur] == 4) {
        cnt += 1;
      }
      int m = cards[cur].back();
      cards[cur].pop_back();
      cur = m;
    }
  }
  cout << cnt << endl;

  return 0;
}
