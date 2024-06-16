#include <ios>
#include <iostream>
#include <vector>
using namespace std;

// trains
int n;
// total round
int total = 20;
// number tobe to push to stack
int cur = 1;

vector<int> output;
vector<int> stack;

void recur() {
  if (total == 0) {
    return;
  }
  if (output.size() == n) {
    total -= 1;
    for (auto &num : output) {
      cout << num;
    }
    cout << endl;
    return;
  }
  // 1. try fetch top stack number to output
  if (stack.size()) {
    output.push_back(stack.back());
    stack.pop_back();
    recur();
    // restore
    stack.push_back(output.back());
    output.pop_back();
  }

  // 2. or push number to stack
  if (cur <= n) {
    stack.push_back(cur);
    cur += 1;

    recur();
    cur -= 1;
    stack.pop_back();
  }
}

int main() {
  ios::sync_with_stdio(false);
  cin >> n;

  recur();

  return 0;
}
