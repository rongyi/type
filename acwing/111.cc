#include <algorithm>
#include <ios>
#include <iostream>
using namespace std;

const int N = 5e5 + 5;

long long input[N];
long long sorted[N];
long long tmp[N];

int n;
int m;
long long t;

void mergesort(int l, int mid, int r) {
  int i = l;
  int j = mid + 1;
  int k = l;
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

// l->mid mid + 1-> r
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
    sum += (tmp[r - i] - tmp[l + i]) * (tmp[r - i] - tmp[l + i]);
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
  std::cin >> round;
  while (round--) {
    cin >> n >> m >> t;

    for (int i = 1; i <= n; i++) {
      cin >> input[i];
      sorted[i] = input[i];
    }

    // now we check
    int chunk = 0;
    // inclusive
    int l = 1;
    int r = n;

    while (l <= r) {
      l = binary(l, r) + 1;
      chunk += 1;
    }

    cout << chunk << endl;
  }

  return 0;
}
