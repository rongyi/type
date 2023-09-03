

#include <deque>
#include <iostream>
#include <vector>
using namespace std;

class Solution {
 public:
  void sort(vector<int> &nums) {

  }
};

int main() {
  Solution so;
  vector<int> input{1, 0, 2, 1, 0};
  so.sort(input);
  for (auto &num : input) {
    cout << num << " ";
  }
  cout << endl;
}

