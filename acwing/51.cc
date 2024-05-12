#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
  vector<vector<int>> permutation(vector<int> &nums) {
    sort(nums.begin(), nums.end());

    auto sz = nums.size();
    vector<int> visited(sz, 0);
    vector<int> cur;
    vector<vector<int>> res;

    recur(nums, 0, cur, visited, res);

    return res;
  }
  void recur(vector<int> &nums, int cur_idx, vector<int> &cur,
             vector<int> &visited, vector<vector<int>> &res) {
    if (cur.size() == nums.size()) {
      res.push_back(cur);
      return;
    }

    for (int i = 0; i < nums.size(); i++) {
      // dedup is pain in the ass
      if (!visited[i] && (i == 0 || nums[i] != nums[i - 1] || visited[i - 1])) {

        visited[i] = 1;
        cur.push_back(nums[i]);
        recur(nums, i + 1, cur, visited, res);
        visited[i] = 0;
        cur.pop_back();
      }
    }
  }
};

int main() {
  Solution so;
  vector<int> input{1, 1, 2};
  auto ret = so.permutation(input);
  for (auto &c : ret) {
    for (auto &num : c) {
      cout << num << " ";
    }
    cout << endl;
  }
}
