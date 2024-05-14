#include "xxx.hpp"
#include <deque>
#include <vector>

class Solution {
public:
  int movingCount(int k, int m, int n) {
    if (m * n == 0) {
      return 0;
    }
    deque<int> q;
    q.push_back(0);

    auto sum = 0;
    vector<vector<int>> dirs{{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    vector<vector<int>> visited = vector<vector<int>>(m, vector<int>(n, 0));
    visited[0][0] = 1;

    while (!q.empty()) {
      auto sz = q.size();
      for (int i = 0; i < sz; i++) {
        auto cur = q.front();
        q.pop_front();
        auto x = cur / n;
        auto y = cur % n;

        sum += 1;

        for (auto &d : dirs) {
          auto nx = x + d[0];
          auto ny = y + d[1];
          if (nx < 0 || nx >= m || ny < 0 || ny >= n || digitsum(nx, ny) > k ||
              visited[nx][ny]) {
            continue;
          }
          visited[nx][ny] = 1;
          q.push_back(nx * n + ny);
        }
      }
    }

    return sum;
  }
  int digitsum(int num, int num2) {
    auto sum = 0;
    while (num) {
      sum += num % 10;
      num /= 10;
    }
    while (num2) {
      sum += num2 % 10;
      num2 /= 10;
    }
    return sum;
  }
};

int main() {
  Solution so;
  so.movingCount(9, 20, 25);
}
