#include <algorithm>
#include <ios>
#include <iostream>

using namespace std;

const int N = 1e6 + 5;
long long candy[N];
long long sum = 0;
long long prefix[N];

int main() {
  ios::sync_with_stdio(false);
  int n;
  cin >> n;
  for (int i = 0; i < n; i++) {
    cin >> candy[i];
    sum += candy[i];
  }
  long long avg = sum / n;

  for (int i = 1; i < n; i++) {
    prefix[i] = prefix[i - 1] + candy[i] - avg;
  }

  sort(prefix, prefix + n);
  long long ret = 0;
  long long mid = prefix[n / 2];
  for (int i = 0; i < n; i++) {
    ret += abs(prefix[i] - mid);
  }
  cout << ret;

  return 0;
}
