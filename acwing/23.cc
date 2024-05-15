#include "xxx.hpp"
#include <vector>

class Solution {
public:
  TreeNode *buildTree(vector<int> &preorder, vector<int> &inorder) {
    // so we can easily get index
    map<int, int> in_idx;
    int sz = inorder.size();
    for (int i = 0; i < sz; i++) {
      in_idx[inorder[i]] = i;
    }

    return do_build(preorder, in_idx, 0, 0, sz - 1);
  }

  TreeNode *do_build(vector<int> &preorder, map<int, int> &in_idx, int cur,
                     int l, int r) {
    if (l > r) {
      return nullptr;
    }
    auto m = in_idx[preorder[cur]];
    TreeNode *ret = new TreeNode(preorder[cur]);
    ret->left = do_build(preorder, in_idx, cur + 1, l, m - 1);
    // + 1 to jump over m
    // m - l -> left subtree size
    ret->right = do_build(preorder, in_idx, cur + 1 + m - l, m + 1, r);

    return ret;
  }
};
