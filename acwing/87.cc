#include "xxx.hpp"

class Solution {
public:
  TreeNode *convert(TreeNode *root) {
    dfs(root);

    while (root && root->left) {
      root = root->left;
    }

    return root;
  }
  void dfs(TreeNode *cur) {
    if (!cur) {
      return;
    }
    dfs(cur->left);

    cur->left = prev;
    if (prev) {
      prev->right = cur;
    }
    prev = cur;

    dfs(cur->right);
  }
  TreeNode *prev = nullptr;
};
