#include <iostream>
using namespace std;

int round = 0;
int r = 0;
int grid[5010][5010] = {};

// 细节还挺多，
// 1. +1 postion弄成prefix形式
// 2. r的值弄到最小   -
int main() {    //  |
  cin >> round; //  |
  cin >> r;     //  |
  r = min(5001, r);
  for (int i = 0; i < round; i++) {
    int x;
    int y;
    int w;
    cin >> x >> y >> w;
    x++;
    y++;
    grid[x][y] += w;
  }
  for (int i = 1; i <= 5001; i++) {
    for (int j = 1; j <= 5001; j++) {
      grid[i][j] += grid[i - 1][j] + grid[i][j - 1] - grid[i - 1][j - 1];
    }
  }
  int max_weight = 0;
  for (int i = r; i <= 5001; i++) {
    for (int j = r; j <= 5001; j++) {
      int val =
          grid[i][j] - grid[i - r][j] - grid[i][j - r] + grid[i - r][j - r];
      max_weight = max(max_weight, val);
    }
  }
  cout << max_weight << endl;
  return 0;
}
