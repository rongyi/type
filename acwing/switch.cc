#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

char grid[5][5];
char backup[5][5];

void flip(int x, int y) {
  vector<vector<int>> dirs{{0, 1}, {1, 0}, {0, -1}, {-1, 0}, {0, 0}};
  for (int i = 0; i < dirs.size(); i++) {
    int nx = x + dirs[i][0];
    int ny = y + dirs[i][1];
    if (nx < 0 || nx >= 5 || ny < 0 || ny >= 5) {
      continue;
    }
    grid[nx][ny] ^= 1;
  }
}

int work() {
  int ret = 1000;
  for (int k = 0; k < (1 << 5); k++) {
    int cur = 0;
    char backup[5][5];
    memcpy(backup, grid, sizeof(grid));

    for (int j = 0; j < 5; j++) {
      if (k >> j & 1) {
        cur += 1;
        flip(0, j);
      }
    }

    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 5; j++) {
        if (grid[i][j] == '0') {
          cur += 1;
          flip(i + 1, j);
        }
      }
    }
    bool succ = true;
    for (int j = 0; j < 5; j++) {
      if (grid[4][j] == '0') {
        succ = false;
        break;
      }
    }
    memcpy(grid, backup, sizeof(grid));
    if (succ) {
      ret = min(ret, cur);
    }
  }
  if (ret <= 6) {
    return ret;
  }
  return -1;
}

int main() {
  int n;
  cin >> n;

  while (n--) {
    for (int i = 0; i < 5; i++) {
      char status;
      for (int j = 0; j < 5; j++) {
        cin >> status;
        grid[i][j] = status;
      }
    }
    cout << work() << endl;
  }
}
