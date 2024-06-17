#include <ios>
#include <iostream>
#include <limits>

using namespace std;

const int N = 105;

int grid[N][N];

int main() {
  ios::sync_with_stdio(false);
  int n;
  cin >> n;
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= n; j++) {
      cin >> grid[i][j];
      // column prefix
      grid[i][j] += grid[i - 1][j];
    }
  }

  // for single 1d array,
  // the max subarray sum is:
  // f[i] = f[i - 1] + nums[i] if f[i - 1] >= 0
  // else
  // f[i] = nums[i]
  // here the last value is like subarray sum in one demension
  int ret = numeric_limits<int>::min();
  for (int i = 1; i <= n; i++) {
    for (int k = i; k <= n; k++) {
      int last = 0;
      for (int j = 1; j <= n; j++) {
        last = max(last, 0) + grid[k][j] - grid[i - 1][j];
        ret = max(ret, last);
      }
    }
  }

  cout << ret << endl;

  return 0;
}
