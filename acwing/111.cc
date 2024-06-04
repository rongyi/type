#include <algorithm>
#include <ios>
#include <iostream>
using namespace std;

const int N = 5e5 + 5;
int input[N];
int sorted[N];
// for mergesort output
int tmp[N];

int n;
int m;
long long t;

void mergesort(int l, int mid, int r) {
  int i = l;
  int k = l;
  int j = mid + 1;
  while (i <= mid && j <= r) {
    if (sorted[i] <= sorted[j]) {
      tmp[k++] = sorted[i++];
    } else {
      tmp[k++] = sorted[j++];
    }
  }
  while (i <= mid) {
    tmp[k++] = sorted[i++];
  }
  while (j <= r) {
    tmp[k++] = sorted[j++];
  }
}

// inclusive
bool check(int l, int mid, int r) {
  for (int i = mid + 1; i <= r; i++) {
    sorted[i] = input[i];
  }
  sort(sorted + mid + 1, sorted + r + 1);
  mergesort(l, mid, r);

  long long sum = 0;

  for (int i = 0; i < m; i++) {
    if (l + i >= r - i) {
      break;
    }
    long long val = (tmp[l + i] - tmp[r - i]);
    sum += val * val;
  }

  return sum <= t;
}

int binary(int l, int r) {
  int p = 1;
  int k = l;
  while (p) {
    if (k + p <= r && check(l, k, k + p)) {
      k += p;
      p *= 2;

      for (int i = l; i <= k; i++) {
        sorted[i] = tmp[i];
      }
    } else {
      p /= 2;
    }
  }
  return k;
}

int main() {
  std::ios::sync_with_stdio(false);
  int round;
  cin >> round;
  while (round--) {
    cin >> n >> m >> t;
    for (int i = 1; i <= n; i++) {
      cin >> input[i];
      sorted[i] = input[i];
    }

    // both inclusive
    int l = 1;
    int r = n;
    int ret = 0;
    while (l <= r) {
      l = binary(l, r) + 1;
      ret += 1;
    }
    cout << ret << endl;
  }

  return 0;
}
