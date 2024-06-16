#include <algorithm>
#include <ios>
#include <iostream>
#include <vector>

using namespace std;

int main() {
  ios::sync_with_stdio(false);
  int t;
  cin >> t;
  while (t--) {
    int origin, target;
    string origin_num;
    cin >> origin >> target >> origin_num;

    vector<int> number;
    for (auto &c : origin_num) {
      if (c >= '0' && c <= '9') {
        number.push_back(c - '0');
      } else if (c >= 'a' && c <= 'z') {
        number.push_back(c - 'a' + 36);
      } else if (c >= 'A' && c <= 'Z') {
        number.push_back(c - 'A' + 10);
      }
    }
    // process from high to low, ensure high is at the end of number
    reverse(number.begin(), number.end());
    vector<int> output;
    while (number.size()) {
      int r = 0;
      for (int i = number.size() - 1; i >= 0; i--) {
        number[i] += r * origin;
        r = number[i] % target;
        number[i] /= target;
      }
      output.push_back(r);
      while (number.size() && number.back() == 0) {
        number.pop_back();
      }
    }
    reverse(output.begin(), output.end());
    string fmt;
    for (auto &num : output) {
      if (num <= 9) {
        fmt.push_back('0' + num);
      } else if (num <= 35) {
        fmt.push_back('A' + num - 10);
      } else {
        fmt.push_back('a' + num - 36);
      }
    }
    cout << origin << " " << origin_num << endl;
    cout << target << " " << fmt << endl;
    cout << endl;
  }

  return 0;
}
