

#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;
int main() {
  int n = 0;
  cin >> n;

  vector<vector<int>> grid(n, vector<int>(n, 0));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      cin >> grid[i][j];
    }
  }
  vector<vector<int>> dp(1 << n, vector<int>(n, 0x3f3f3f3f));
  dp[1][0] = 0;

  for (int cur_state = 1; cur_state < (1 << n); cur_state++) {
    // 以i为end节点
    for (int i = 0; i < n; i++) {
      // 那么至少i节点要在这个全局状态里面嘛
      if ((cur_state >> i) & 1) {
        // 假设最后一跳是从j走过来到达i，状态转移方程
        auto withouti_state = cur_state ^ (1 << i);
        for (int j = 0; j < n; j++) {
          if ((withouti_state >> j) & 1) {
            dp[cur_state][i] =
                min(dp[cur_state][i], dp[withouti_state][j] + grid[j][i]);
          }
        }
      }
    }
  }
  cout << dp[(1 << n) - 1][n - 1] << endl;

  return 0;
}
