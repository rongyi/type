#include "xxx.hpp"

class Solution {
public:
  bool isSymmetric(TreeNode *root) {
    if (!root) {
      return true;
    }

    return down_check(root->left, root->right);
  }

  bool down_check(TreeNode *l, TreeNode *r) {
    auto empty_l = l == nullptr;
    auto empty_r = r == nullptr;
    if (empty_l ^ empty_r) {
      return false;
    }
    if (empty_l) {
      return true;
    }
    // two non empty
    return l->val == r->val && down_check(l->left, r->right) &&
           down_check(l->right, r->left);
  }
};
