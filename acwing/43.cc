#include <deque>
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
  vector<int> printFromTopToBottom(TreeNode *root) {
    if (!root) {
      return {};
    }
    vector<int> res;
    deque<TreeNode *> q;
    q.push_back(root);
    while (!q.empty()) {
      auto sz = q.size();
      for (int i = 0; i < sz; i++) {
        auto cur = q.front();
        q.pop_front();
        res.push_back(cur->val);
        if (cur->left) {
          q.push_back(cur->left);
        }
        if (cur->right) {
          q.push_back(cur->right);
        }
      }
    }

    return res;
  }
};
