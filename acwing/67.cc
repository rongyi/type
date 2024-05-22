#include "xxx.hpp"
#include <algorithm>

class Solution {
public:
  int treeDepth(TreeNode *root) {
    if (!root) {
      return 0;
    }
    auto l = treeDepth(root->left);
    auto r = treeDepth(root->right);
    return 1 + max(l, r);
  }
};
