#include <iostream>
#include <vector>

using namespace std;

int visited[10];
int choose[10];

void do_permutation(int n, vector<int> &visited, vector<int> &choose) {
  if (choose.size() >= n) {
    for (auto &num : choose) {
      cout << num << " ";
    }
    cout << endl;
    return;
  }

  for (int i = 1; i <= n; i++) {
    if (visited[i]) {
      continue;
    }
    visited[i] = 1;
    choose.push_back(i);

    do_permutation(n, visited, choose);

    choose.pop_back();
    visited[i] = 0;
  }
}

int main() {
  int n;
  cin >> n;
  vector<int> visited(10, false);
  vector<int> choose;
  do_permutation(n, visited, choose);
}
