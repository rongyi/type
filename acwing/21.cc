#include "xxx.hpp"
#include <vector>

class Solution {
public:
  bool hasPath(vector<vector<char>> &matrix, string &str) {
    if (matrix.empty()) {
      return false;
    }
    int m = matrix.size();
    if (matrix[0].empty()) {
      return false;
    }
    int n = matrix[0].size();
    for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
        vector<vector<int>> visited(m, vector<int>(n, 0));
        visited[i][j] = 1;
        if (dfs(matrix, visited, str, i, j, 0)) {
          return true;
        }
        // visited[i][j] = 0;
      }
    }
    return false;
  }

  bool dfs(vector<vector<char>> &matrix, vector<vector<int>> &visited,
           string &s, int x, int y, int idx) {
    if (s[idx] != matrix[x][y]) {
      return false;
    }

    if (idx == s.size() - 1) {
      return true;
    }

    for (auto &d : dirs) {
      auto nx = x + d[0];
      auto ny = y + d[1];
      if (nx < 0 || nx >= matrix.size() || ny < 0 || ny >= matrix[0].size() ||
          visited[nx][ny]) {
        continue;
      }
      visited[nx][ny] = 1;
      if (dfs(matrix, visited, s, nx, ny, idx + 1)) {
        return true;
      }
      visited[nx][ny] = 0;
    }

    return false;
  }

private:
  vector<vector<int>> dirs = {{0, 1}, {1, 0}, {-1, 0}, {0, -1}};
};

int main() {
  Solution so;
  vector<vector<char>> input{
      {'A', 'B', 'C', 'E'}, {'S', 'F', 'E', 'S'}, {'A', 'D', 'E', 'E'}};
  string s{"ABCEFSADEESE"};
  auto ret = so.hasPath(input, s);
  cout << ret << endl;
}
