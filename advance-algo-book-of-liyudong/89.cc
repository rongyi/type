#include <iostream>
using namespace std;

long long comput(int a, int b, int p) {
  long long ret = 1 % p;
  for (; b; b >>= 1) {
    if (b & 1) {
      ret = (ret * a) % p;
    }
    a = (long long)a * a % p;
  }

  return ret;
}

int main() {
  int a;
  int b;
  int p;

  cin >> a;
  cin >> b;
  cin >> p;

  cout << comput(a, b, p);
}
