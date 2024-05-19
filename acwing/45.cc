#include "xxx.hpp"
#include <vector>

class Solution {
public:
  vector<vector<int>> findPath(TreeNode *root, int sum) {
    dfs(root, 0, sum, {});

    return paths;
  }
  void dfs(TreeNode *cur, int sum, int target, vector<int> path) {
    if (!cur) {
      return;
    }
    sum += cur->val;
    path.push_back(cur->val);
    // root -> leaf
    if (sum == target && !cur->left && !cur->right) {
      paths.push_back(path);
    }
    dfs(cur->left, sum, target, path);
    dfs(cur->right, sum, target, path);
  }

private:
  vector<vector<int>> paths;
};
