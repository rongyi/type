#include <algorithm>
#include <vector>

using namespace std;

/**
 * Definition for a binary tree node.
 */

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
public:
  vector<vector<int>> printFromTopToBottom(TreeNode *root) {
    vector<vector<int>> res;
    dfs(root, 1, res);

    for (int i = 1; i < res.size(); i += 2) {
      reverse(res[i].begin(), res[i].end());
    }

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
