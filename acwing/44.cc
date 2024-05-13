#include "xxx.hpp"

class Solution {
public:
  vector<vector<int>> printFromTopToBottom(TreeNode *root) {
    vector<vector<int>> res;
    dfs(root, 1, res);

    return res;
  }

  void dfs(TreeNode *cur, int depth, vector<vector<int>> &res) {
    if (!cur) {
      return;
    }
    if (res.size() < depth) {
      res.push_back({});
    }
    res[depth - 1].push_back(cur->val);

    dfs(cur->left, depth + 1, res);
    dfs(cur->right, depth + 1, res);
  }
};
