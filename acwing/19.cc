

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode *father;
  TreeNode(int x) : val(x), left(nullptr), right(nullptr), father(nullptr) {}
};

class Solution {
public:
  TreeNode *inorderSuccessor(TreeNode *p) {
    if (!p) {
      return nullptr;
    }
    auto right = p->right;
    // we are lucky
    if (right) {
      while (right->left) {
        right = right->left;
      }
      return right;
    } else {
      if (p->father == nullptr) {
        return nullptr;
      }
      // 1. i am the left child of parent
      // simply return father
      if (p == p->father->left) {
        return p->father;
      }
      auto cur = p->father;
      while (cur != nullptr && cur->father != nullptr &&
             cur->father->right == cur) {
        cur = cur->father;
      }
      return cur->father;
    }
  }
};
