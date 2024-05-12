#include <cstring>
#include <iostream>
using namespace std;

int main() {
  const int sz = 13;
  // 4 根柱子
  int f[sz];
  // 3 根柱子
  int d[sz];
  d[1] = 1;
  for (int i = 2; i <= 12; i++) {
    d[i] = d[i - 1] * 2 + 1;
  }
  memset(f, 0x3f, sizeof(f));

  f[1] = 1;
  for (int i = 2; i <= 12; i++) {
    for (int j = 1; j < i; j++) {
      // 取j到别的柱子上，剩余i - j是三根柱子的问题
      f[i] = min(f[i], f[j] * 2 + d[i - j]);
    }
  }

  for (int i = 1; i <= 12; i++) {
    cout << f[i] << endl;
  }
}
