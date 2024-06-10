#include <iostream>
using namespace std;

//  pow(3, 7)
const int N = 2190;

int grid[N][N];
void draw(int len, int x, int y) {
  if (len == 1) {
    grid[x][y] = 1;
    return;
  }
  // top left
  draw(len / 3, x, y);
  // top right
  draw(len / 3, x, y + len / 3 * 2);
  // center
  draw(len / 3, x + len / 3, y + len / 3);

  // down left
  draw(len / 3, x + len / 3 * 2, y);
  draw(len / 3, x + len / 3 * 2, y + len / 3 * 2);
}

int main() {
  ios::sync_with_stdio(false);

  int n;
  while (true) {
    cin >> n;
    if (n == -1) {
      return 0;
    }
    int len = 1;
    for (int i = 1; i < n; i++) {
      len *= 3;
    }
    draw(len, 0, 0);
    for (int i = 0; i < len; i++) {
      for (int j = 0; j < len; j++) {
        if (grid[i][j] == 1) {
          cout << "X";
        } else {
          cout << " ";
        }
      }
      cout << endl;
    }

    cout << "-" << endl;
  }

  return 0;
}
