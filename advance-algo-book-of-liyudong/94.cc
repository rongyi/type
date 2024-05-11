#include <iostream>
#include <vector>

using namespace std;

void do_permutation(int n, int m, int i, vector<int> &visited,
                    vector<int> &choose) {
  if (choose.size() > m || (choose.size() + (n - i + 1) < m)) {
    return;
  }

  if (choose.size() == m) {
    for (auto &num : choose) {
      cout << num << " ";
    }
    cout << endl;
    return;
  }

  for (; i <= n; i++) {
    if (visited[i]) {
      continue;
    }
    visited[i] = 1;
    choose.push_back(i);

    do_permutation(n, m, i, visited, choose);

    choose.pop_back();
    visited[i] = 0;
  }
}

int main() {
  int n;
  cin >> n;
  int m;
  cin >> m;
  vector<int> visited(n + 1, false);
  vector<int> choose;
  do_permutation(n, m, 1, visited, choose);
}
