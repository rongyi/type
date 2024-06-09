#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
using namespace std;

// Forward declaration of compare API.
// bool compare(int a, int b);
// return bool means whether a is less than b.

class Solution {
public:
  vector<int> specialSort(int N) {
    vector<int> idx(N, 0);
    iota(idx.begin(), idx.end(), 1);
    stable_sort(idx.begin(), idx.end(), compare);

    return idx;
  }
};
