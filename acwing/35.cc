#include "xxx.hpp"

class Solution {
public:
  bool hasSubtree(TreeNode *p1, TreeNode *p2) {
    if (!p2) {
      return false;
    }
    if (!p1) {
      return p2 == nullptr;
    }
    if (exactyMatch(p1, p2)) {
      return true;
    }
    return hasSubtree(p1->left, p2) || hasSubtree(p1->right, p2);
  }

  bool exactyMatch(TreeNode *p1, TreeNode *p2) {
    if (!p2) {
      return true;
    }
    if (!p1) {
      return false;
    }

    if (p1->val != p2->val) {
      return false;
    }

    return exactyMatch(p1->left, p2->left) && exactyMatch(p1->right, p2->right);
  }
};
