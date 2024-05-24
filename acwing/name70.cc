#include "xxx.hpp"

class Solution {
public:
  TreeNode *kthNode(TreeNode *root, int k) {
    inorder(root, k);

    return target_;
  }

private:
  void inorder(TreeNode *cur, int k) {
    if (!cur) {
      return;
    }
    inorder(cur->left, k);
    // visit
    visit_cnt_ += 1;
    if (visit_cnt_ == k) {
      target_ = cur;
      return;
    }

    inorder(cur->right, k);
  }

private:
  TreeNode *target_;
  int visit_cnt_ = 0;
};
