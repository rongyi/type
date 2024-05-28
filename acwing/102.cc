#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;
int main() {
  std::vector<long long> nums;
  int n = 0;
  std::cin >> n;
  for (int i = 0; i < n; i++) {
    int val = 0;
    std::cin >> val;
    nums.push_back(val);
  }
  for (int i = nums.size() - 1; i >= 1; i--) {
    nums[i] -= nums[i - 1];
  }

  long long pos_sum = 0;
  long long neg_sum = 0;

  // ignore the leading value
  for (int i = 1; i < nums.size(); i++) {
    auto num = nums[i];
    if (num >= 0) {
      pos_sum += num;
    } else {
      neg_sum += num;
    }
  }
  neg_sum = -neg_sum;

  cout << max(pos_sum, neg_sum) << endl;
  cout << abs(pos_sum - neg_sum) + 1 << endl;
}
