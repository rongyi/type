#include <iostream>
#include <vector>
using namespace std;

void pick(int total, int i, vector<int> &picked) {
  if (i == total + 1) {
    for (auto &num : picked) {
      cout << num << " ";
    }
    cout << endl;
    return;
  }

  // don't pick current
  pick(total, i + 1, picked);
  // pick current
  picked.push_back(i);
  pick(total, i + 1, picked);
  picked.pop_back();
}

int main() {
  int n;
  cin >> n;
  vector<int> picked;
  pick(n, 1, picked);
}
