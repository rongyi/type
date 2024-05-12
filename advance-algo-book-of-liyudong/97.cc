#include <iostream>

using namespace std;

const int mod = 9901;

int qmi(int a, int b) {
  int ret = 1;
  for (; b; b >>= 1) {
    if (b & 1) {
      ret = (long long)ret * a % mod;
    }
    a = (long long)a * a % mod;
  }

  return ret;
}

// calculate 1 + p^0 + p^1 + ... + p^c
int sum_power(int p, int c) {
  if (c == 0) {
    return 1;
  }

  if (c % 2 == 0) {
    // 1 + p + p^2 + ...
    // 1 + p (1 + p + p^2 + .. + p^(c - 1)) c - 1 is odd
    return (p % mod * (sum_power(p, c - 1)) % mod + 1) % mod;
  }

  return sum_power(p, c / 2) * (1 + qmi(p, c / 2 + 1)) % mod;
}

int main() {
  int a;
  int b;
  cin >> a;
  cin >> b;
  int ret = 1;
  for (int i = 2; i <= a; i++) {
    int s = 0;
    while (a % i == 0) {
      s += 1;
      a /= i;
    }
    if (s) {
      ret = ret * sum_power(i, s * b) % mod;
    }
  }
  if (!a) {
    ret = 0;
  }
  cout << ret << endl;
}
