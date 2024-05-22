#include "xxx.hpp"
#include <algorithm>

class Solution {
public:
  bool isBalanced(TreeNode *root) {
    if (!root) {
      return true;
    }
    auto l = height(root->left);
    auto r = height(root->right);

    return abs(l - r) <= 1 && isBalanced(root->left) && isBalanced(root->right);
  }

  int height(TreeNode *node) {
    if (!node) {
      return 0;
    }
    auto l = height(node->left);
    auto r = height(node->right);

    return 1 + max(l, r);
  }
};
