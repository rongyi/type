#include <iostream>
using namespace std;

long long calc(long long a, long long b, long long p) {
  long long ret = 0;
  for (; b; b >>= 1) {
    if (b & 1) {
      ret = (ret + a) % p;
    }
    a = (a * 2) % p;
  }

  return ret;
}

int main() {
  long long a;
  long long b;
  long long p;
  cin >> a;
  cin >> b;
  cin >> p;

  cout << calc(a, b, p) << endl;
}
