#include <cmath>
#include <iostream>
#include <utility>

using namespace std;
using ll = long long;
using pll = pair<ll, ll>;

// n 多少阶城市
// block编号 start from 0
// https://www.acwing.com/solution/content/56398/
pll calc(ll n, ll m) {
  if (n == 0) {
    return {0, 0};
  }
  // 本等级象限 边长/2 也即分四个小块的长度
  auto len = 1ll << (n - 1);
  // 本等级容量，个数
  auto cnt = 1ll << (2 * n - 2);
  // 取余的意思就是，具体在当前的哪个象限不重要，重要的是相对位置
  auto prev_pos = calc(n - 1, m % cnt);
  auto x = prev_pos.first;
  auto y = prev_pos.second;

  auto cur_section = m / cnt;
  if (cur_section == 0) {
    // 左上象限顺转90°（y,-x）沿y对称（-y,-x）更换原点（-y-len,-x+len）
    return {-y - len, -x + len};
  } else if (cur_section == 1) {
    // 仅仅是平移
    return {x + len, y + len};
  } else if (cur_section == 2) {
    // 仅仅是平移
    return {x + len, y - len};
  }
  // 左下象限逆转90°（-y,x）沿y对称（y,x）更换原点（y-len,x-len）
  return {y - len, x - len};
}

int main() {
  int n;
  cin >> n;
  while (n--) {
    ll n;
    ll p1;
    ll p2;
    cin >> n >> p1 >> p2;
    auto pos1 = calc(n, p1 - 1);
    auto pos2 = calc(n, p2 - 1);
    auto dx = (double)(pos1.first - pos2.first);
    auto dy = (double)(pos1.second - pos2.second);
    printf("%.0lf\n", sqrt(dx * dx + dy * dy) * 5);
  }
}
