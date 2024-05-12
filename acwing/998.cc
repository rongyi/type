#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;

int calc(int idx, int v, vector<pair<string, int>> &totals) {
  for (auto &gate : totals) {
    int now = gate.second >> idx & 1;
    if (gate.first == "AND") {
      v &= now;
    } else if (gate.first == "OR") {
      v |= now;
    } else {
      v ^= now;
    }
  }

  return v;
}

int main() {
  int gates;
  cin >> gates;
  int range;
  cin >> range;

  vector<pair<string, int>> totals;
  for (int i = 0; i < gates; i++) {
    char str[5];
    int cur;
    scanf("%s %d", str, &cur);
    totals.push_back({str, cur});
  }

  int val = 0;
  int ret = 0;
  for (int i = 29; i >= 0; i--) {
    //  try put 0 and 1 in this position respectively
    int put0 = calc(i, 0, totals);
    int put1 = calc(i, 1, totals);
    if (val + (1 << i) <= range && put0 < put1) {
      val += 1 << i;
      ret |= put1 << i;
    } else {
      ret |= put0 << i;
    }
  }
  cout << ret << endl;
}
