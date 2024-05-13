#include <iostream>
#include <map>
#include <vector>

using namespace std;

class Solution {
public:
  vector<int> numberOfDice(int n) {
    map<int, int> prev;
    for (int i = 1; i <= 6; i++) {
      prev[i] = 1;
    }
    while (--n) {
      map<int, int> cur;
      for (int i = 1; i <= 6; i++) {
        for (auto &kv : prev) {
          cur[kv.first + i] += kv.second;
        }
      }
      // for (auto &kv : cur) {
      //   cout << "sum: " << kv.first << "-> " << kv.second << endl;
      // }
      // cout << "===" << endl;

      prev.swap(cur);
    }
    vector<int> values;
    for (auto &kv : prev) {
      values.push_back(kv.second);
    }

    return values;
  }
};

int main() {
  Solution so;
  auto res = so.numberOfDice(3);
  for (auto &num : res) {
    cout << num << " ";
  }
  cout << endl;
}
