#include <iostream>
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
  TreeNode *lowestCommonAncestor(TreeNode *root, TreeNode *p, TreeNode *q) {
    if (!root) {
      return nullptr;
    }
    if (p == root || q == root) {
      return root;
    }
    auto p_in_left = find_in_left(root->left, p);
    auto q_in_left = find_in_left(root->left, q);
    // at diff
    if (p_in_left ^ q_in_left) {
      return root;
    }
    if (p_in_left && q_in_left) {
      return lowestCommonAncestor(root->left, p, q);
    }
    return lowestCommonAncestor(root->right, p, q);
  }

  bool find_in_left(TreeNode *node, TreeNode *target) {
    if (!node) {
      return false;
    }
    // pointer address compare, not value!
    if (node == target) {
      return true;
    }
    return find_in_left(node->left, target) ||
           find_in_left(node->right, target);
  }
};
